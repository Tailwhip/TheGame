import socket as st
import selectors
import os
import sys
import argparse
from threading import Thread, RLock
import traceback
import struct
from typing import Any, Callable, Concatenate, ParamSpec, TypeVar
from time import sleep
from queue import Queue, Empty
from functools import wraps

from ..logger import logger
from ..signal_handler import *
from .protocol import *

P = ParamSpec("P")
R = TypeVar("R")


def lock_mutex(
    func: Callable[Concatenate[Any, P], R],
) -> Callable[Concatenate[Any, P], R]:
    """Serialize calls to ``func`` using the instance's ``_mutex`` lock."""
    @wraps(func)
    def _lock_mutex(self: Any, *args: P.args, **kwargs: P.kwargs) -> R:
        with self._mutex:
            return func(self, *args, **kwargs)

    return _lock_mutex


@dataclass(init=True, eq=True)
class Client:
    id: int = 0
    address: tuple[str, int]=("", 0)
    is_registered: bool = False
    message_to_send: Message = Message()

class TgServer:
    def __init__(self):
        self.run = False
        self.sending_queue = Queue()
        self.receiving_queue = Queue()
        self._sending_thread = Thread()
        self._receiving_thread = Thread()
        self._listening_port = 7777
        self._max_clients = 10
        self._server_socket = st.socket(st.AF_INET, st.SOCK_DGRAM)
        self._clients = {}
        self._mutex = RLock()
        self._stopped = False


    def __del__(self):
        try:
            self.stop_server()
        except Exception as e:
            logger.error(f"An error has occured while stopping server: {e}")


    @property
    def clients(self) -> Dict:
        return self._clients

    def start_server(self) -> None:
        logger.debug("Starting the server's threads")
        self.run = True
        self._server_socket.setsockopt(st.SOL_SOCKET, st.SO_REUSEADDR, 1)
        self._server_socket.bind(("127.0.0.1", self._listening_port))
        self._server_socket.setblocking(False)
        self._receiving_thread = Thread(
            target=self.__receiving_loop, args=[], daemon=True)
        self._receiving_thread.start()
        sleep(0.5)
        self._sending_thread = Thread(
            target=self.__sending_loop, args=[], daemon=True)
        self._sending_thread.start()


    def stop_server(self):
        with self._mutex:
            if self._stopped:
                return
            self._stopped = True
            self.run = False
            logger.debug("Stopping the server's threads")
            clients = list(self._clients.values())

        for client in clients:
            logger.debug(f"Stopping client: {client.id}")
            self.send_data(
                address=client.address,
                message=Message(
                    client_id=client.id,
                    message_type=msg_type["Unregister"],
                    signal=Signal(1, 0, 0x01)))

        for name, thread in (("sending", self._sending_thread),
                             ("receiving", self._receiving_thread)):
            if thread.is_alive():
                logger.debug(f"Joining {name} thread")
                thread.join(timeout=5 * STEP_TIME_S)
                if thread.is_alive():
                    logger.warning(f"{name} thread did not stop in time")

        try:
            self._server_socket.close()
        except Exception as e:
            logger.warning(f"An exception has occured while closing: {e}")
        logger.debug("Server stopped")
        
        
    @lock_mutex
    def receive_data(self):
        data = b'\x00'
        address = ("", 0)
        try:
            data, address = self._server_socket.recvfrom(MAX_DATA_SIZE)
            if not data:
                logger.debug("No data received")
                return False, data, address
        except BlockingIOError:
            # Non-blocking socket with nothing queued: this is normal.
            return False, data, address
        except OSError as e:
            if self.run:
                logger.error(f"An exception has occured: {e}")
            return False, data, address
        return True, data, address

    
    def __receiving_loop(self):
        logger.debug("")
        while self.run:
            sleep(STEP_TIME_S)
            result, data, address = self.receive_data()
            logger.debug(f"Result: {result}")
            if result is False:
                continue
            else:
                msg = Message()
                msg.deserialize(data)
                self.__handle_new_message(address, msg)
            logger.debug("Received data from connected user: " + str(data))   
    

    def __handle_new_message(self, address: Any, message: Message):
        logger.debug("")
        client = Client()
        if msg_type["Register"] == message.message_type:
            client.id = message.client_id
            client.address = address
            if not message.client_id in self._clients:
                self._clients[message.client_id] = client
                logger.debug(
                    f"Client {client.address} has been registered with id {client.id} successfully")
            else:
                logger.debug(
                    f"Client {client.address} with id {client.id} is already registered!")
            self.send_data(
                address=client.address,
                message=Message(
                    client_id=client.id,
                    message_type=msg_type["Register"],
                    signal=Signal(1, 0, 0x01)))
        elif message.client_id in self._clients:
            client = self._clients[message.client_id]
            self.receiving_queue.put({client.id, message})
            logger.debug(f"New message has been received: {message}")
        else:
            logger.warning(
                f"Client {client.address} with id {client.id} is unregistered!")
        
            
    @lock_mutex
    def send_data(self, address, message: Message=None):
        if (message is not None):
            data_bytes = message.serialize()
        else:
            logger.error("No message to be send!")
            return
        try:
            logger.debug(f"Sending data: {data_bytes} to client {address}")
            if not self._server_socket.sendto(data_bytes, address):
                logger.error("Failed to send the message!")
        except Exception as e:
            logger.error(f"An exception has occured: {e}")


    def __sending_loop(self):
        while self.run:
            try:
                message = self.sending_queue.get(timeout=STEP_TIME_S)
            except Empty:
                continue
            client = self._clients.get(message.client_id)
            if client is not None and client.id and message.message_type != 0:
                self.send_data(client.address, message)
                logger.debug("Data has been sent")
            else:
                logger.warning(
                    f"Client {message.client_id} is unregistered or message to send is empty!")


    def __connect_to_client(self, host = "127.0.0.1", port = 6969):
        logger.debug("Connecting to server...")
        server_address = host, port
        socket = st.socket(st.AF_INET, st.SOCK_DGRAM)
        socket.setblocking(False)
        socket.connect_ex(server_address)
        return socket

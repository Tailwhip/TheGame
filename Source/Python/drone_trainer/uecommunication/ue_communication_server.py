import socket as st
import selectors
import os
import sys
import argparse
from threading import Thread, Lock
import traceback
import struct
from typing import Any, Dict
from time import sleep
from queue import Queue

from ..logger import logger
from ..signal_handler import *
from .message import *


MAX_DATA_SIZE = 1024


@dataclass(init=True, eq=True)
class Client:
    id: int = 0
    address: tuple[str, int]=("", 0)
    is_registered: bool = False
    message_to_send: Message = Message
    

class UE5Server:
    def __init__(self):
        self.run = False
        self.sending_queue = Queue()
        self.receiving_queue = Queue()
        self._sending_thread = Thread()
        self._receiving_thread = Thread()
        self._connection_thread = Thread()
        self._listening_port = 7777
        self._max_clients = 10
        self._server_socket = st.socket(st.AF_INET, st.SOCK_DGRAM)
        self._clients = {}
        self._mutex = Lock()

    @property
    def clients(self) -> Dict:
        return self._clients

    def __lock_mutex(func) -> Any:
        def lock_mutex(self, *args, **kwargs):
            self._mutex.acquire()
            result = func(self, *args, **kwargs)
            self._mutex.release()
            return result
        return lock_mutex


    def start_server(self) -> None:
        logger.info("Starting the server's threads")
        self.run = True
        self._server_socket.bind(("127.0.0.1", self._listening_port))
        self._server_socket.setsockopt(st.SOL_SOCKET, st.SO_REUSEADDR, 1)
        self._server_socket.setblocking(False)
        self._receiving_thread = Thread(target=self.__receiving_loop, args=[])
        self._receiving_thread.start()
        sleep(0.5)
        self._sending_thread = Thread(target=self.__sending_loop, args=[])
        self._sending_thread.start()
    
    
    def __wait_for_connection(self):
        while self.run:
            logger.info("Waiting for connection from clients...")
            # configure how many client the server can listen simultaneously
            # self._server_socket.listen(self._max_clients)
            client = Client()
            # client.connection, client.address = self._server_socket.accept()  # accept new connection
            self._clients[client.id] = client
            logger.info("Connection from: " + str(client.address))
            sleep(1)
    
    
    @__lock_mutex
    def stop_server(self):
        logger.info("Stopping the servers's thread")
        self.run = False
        try:
            logger.info("Joining sending thread")
            self._sending_thread.join()
            logger.info("Joining receiving thread")
            self._receiving_thread.join()
            self._server_socket.close()
        except Exception as e:
            logger.warning(f"An exception has occured: {e}")
        
        
    @__lock_mutex
    def receive_data(self):
        logger.info("")
        data = b'\x00'
        address = ("", 0)
        try:
            data, address = self._server_socket.recvfrom(MAX_DATA_SIZE)
            if not data:
                logger.info("No data received")
                return False, data, address
        except Exception as e:
            logger.error(f"An exception has occured: {e}")
            return False, data, address
        
        return True, data, address

    
    def __receiving_loop(self):
        logger.info("")
        while self.run:
            sleep(1)
            result, data, address = self.receive_data()
            logger.info(f"Result: {result}")
            if result is False:
                continue
            else:
                msg = Message()
                msg.deserialize(data)
                self.__handle_new_message(address, msg)
            logger.info("Received data from connected user: " + str(data))   
    
    
    def __handle_new_message(self, address: any, message: Message):
        logger.info("")
        client = Client()
        if msg_type["Register"] == message.message_type:
            client.id = message.client_id
            client.address = address
            if not message.client_id in self._clients:
                self._clients[message.client_id] = client
                logger.info(
                    f"Client {client.address} has been registered with id {client.id} successfully")
            else:
                logger.info(
                    f"Client {client.address} with id {client.id} is already registered!")
            self.send_data(
                address=client.address,
                message=Message(
                    client_id=0,
                    message_type=msg_type["Register"],
                    signal=Signal(1, 0, 0x01)))
        elif message.client_id in self._clients:
            client = self._clients[message.client_id]
            self.receiving_queue.put({client.id, message})
        else:
            logger.warning(
                f"Client {client.address} with id {client.id} is unregistered!")
        
            
    @__lock_mutex
    def send_data(self, address, message: Message=None):
        logger.info("")
        if (message is not None):
            data_bytes = message.serialize()
        else:
            logger.info("No message to be send")
            return
        try:
            logger.info(f"Sending data: {data_bytes} to client {address}")
            if not self._server_socket.sendto(data_bytes, address):
                logger.error("Failed to send the message!")
        except Exception as e:
            logger.error(f"An exception has occured: {e}")


    def __sending_loop(self):
        logger.info("")
        while self.run:
            sleep(1)
            message = self.sending_queue.get()
            client = self._clients[message.client_id]
            if client.id and message.message_type != 0:
                self.send_data(client.address, message)
                logger.info("Data has been sent")
            else:
                logger.warning(f"Client {client.id} is unregistered or message to send is empty!")


    def __connect_to_client(self, host = "127.0.0.1", port = 6969):
        logger.info("Connecting to server...")
        server_address = host, port
        socket = st.socket(st.AF_INET, st.SOCK_DGRAM)
        socket.setblocking(False)
        socket.connect_ex(server_address)
        return socket

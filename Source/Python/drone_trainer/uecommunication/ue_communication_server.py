import socket as st
import selectors
import os
import sys
import argparse
from threading import Thread, Lock
import traceback
from time import sleep
from queue import Queue

from ..logger import logger
from ..signal_handler import *


class UE5Server:
    def __init__(self):
        self.run = False
        self.sending_queue = Queue()
        self.receiving_queue = Queue()
        self.connected_clients = {}
        self.message_to_send = "Server's message"
        self.response_message = "Server's response"
        self._sending_thread = Thread()
        self._receiving_thread = Thread()
        self._connection_thread = Thread()
        self._listener_port = 7777
        self._sending_port = 7776
        self._max_clients = 10
        self._server_socket = st.socket()
        self._conn = None
        self._address = None
        self._mutex = Lock()


    def __lock_mutex(func):
        def lock_mutex(self, *args, **kwargs):
            self._mutex.acquire(blocking=False)
            result = func(self, *args, **kwargs)
            self._mutex.release()
            return result
        return lock_mutex


    def start_server(self):
        logger.info("Starting the server's threads")
        self.run = True
        
        # bind host address and port together
        self._server_socket.bind(("127.0.0.1", self._listener_port))  
        # self._server_socket.setblocking(False)

        # conn, address = self._server_socket.accept()  # accept new connection
        self._connection_thread = Thread(target=self.__wait_for_connection, args=[])
        self._connection_thread.start()
        sleep(0.5)
        self._receiving_thread = Thread(target=self.__receiving_loop, args=[])
        self._receiving_thread.start()
        sleep(0.5)
        self._sending_thread = Thread(target=self.__sending_loop, args=[])
        self._sending_thread.start()
    
    
    def __wait_for_connection(self):
        while self.run:
            logger.info("Waiting for connection from clients...")
            # configure how many client the server can listen simultaneously
            self._server_socket.listen(self._max_clients)
            self._conn, self._address = self._server_socket.accept()  # accept new connection
            logger.info("Connection from: " + str(self._address))
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
            logger.info("Joining connection thread")
            self._server_socket.close()
            self._connection_thread.join()
        except Exception as e:
            logger.warning(f"An exception has occured: {e}")
        
        
    @__lock_mutex
    def receive_data(self, conn):
        logger.info("")
        data = ""
        try:
            if(conn):
                # receive data stream. it won't accept data packet greater than 1024 bytes
                data = conn.recv(1024).decode('utf-8')
                if not data:
                    logger.info("No data received")
                    # if data is not received break
                    return False, data
                return True, data
            else:
                logger.error("Invalid connection!")
                return False, data
        except Exception as e:
            logger.error(f"An exception has occured: {e}")
            return False, data

    
    def __receiving_loop(self):
        logger.info("")
        while self.run:
            sleep(1)
            logger.info("Receiving data...")
            result, data = self.receive_data(self._conn)
            logger.info(f"Result: {result}")
            if result is False:
                continue
            logger.info("Received data from connected user: " + str(data))
        if self._conn:
            self._conn.close()  # close the connection
    
    
    @__lock_mutex
    def send_data(self, conn, data=None):
        logger.info("")
        # Sends to the socket
        if (data):
            data_bytes = data.encode('utf-8')
        else:
            data_bytes = self.message_to_send.encode('utf-8')

        try:
            if(conn):
                conn.send(data_bytes)  # send data to the client
            else:
                logger.error("Invalid connection!")
        except Exception as e:
            logger.error(f"An exception has occured: {e}")


    def __sending_loop(self):
        logger.info("")
        while self.run:
            sleep(1)
            logger.info("Sending data...")
            self.send_data(self._conn)
        if self._conn:
            self._conn.close()  # close the connection

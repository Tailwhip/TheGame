import socket as st
import selectors
import os
import sys
import argparse
import threading
import traceback
from time import sleep

from ..logger import logger
from ..signal_handler import *


class UE5Client:
    def __init__(self):
        self.run = False
        self._data = None
        self._thread = None

    def start_client(self):
        logger.info("Starting the client's thread")
        self.run = True
        self._thread = threading.Thread(target=self.__client_loop, args=[])
        self._thread.start()
        
    def stop_client(self):
        logger.info("Stopping the client's thread")
        self.run = False
        self._thread.join()
    
    def __connect_to_server(self, host = "127.0.0.1", port = 7777):
        # Starts the client
        logger.info("Connecting to server...")
        server_address = host, port
        socket = st.socket(st.AF_INET, st.SOCK_STREAM)
        socket.setblocking(False)
        socket.connect_ex(server_address)
        return socket

    def send_data(self, data="dummy_data"):
        # Sends to the socket
        data_bytes = data.encode('utf-8')
        socket = self.__connect_to_server()
        try:
            if(socket):
                logger.info(f"Sending data: {data_bytes}")
                socket.sendall(data_bytes)
                logger.info(f"Data sent succesfully")
            else:
                logger.error("Invalid socket!")
        except Exception as e:
            logger.error(f"An exception has occured: {e}")
        finally:
            socket.close()
    
    def __client_loop(self):
        while self.run:
            sleep(1)
            data = "Hello The Game!"
            self.send_data(data)


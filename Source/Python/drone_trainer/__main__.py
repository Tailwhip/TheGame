#!/usr/bin/env python

import signal
import sys
import time

from .uecommunication.ue_communication_server import UE5Server
from .signal_handler import *
from .logger import logger


def main():
    logger.info(f"Drone Trainer has started")
    
    try:
        # client = UE5Client()
        # client.start_client()
        # while client.run:
        #     time.sleep(1)
        logger.info(f"Starting UE communication server")
        server = UE5Server()
        server.start_server()
        
        signal_handler = SignalHandler(server.stop_server)
        signal.signal(signal.SIGINT, signal_handler)
        while server.run:
            time.sleep(1)
        
    except Exception as e:
        logger.error(f"An error has occured: {e}")
    except KeyboardInterrupt:
        logger.error(f"Keyboard interrupted")
    finally:
        logger.info(f"Finally...")
                
    logger.info(f"Drone Trainer exit")


if __name__ == "__main__":
    main()

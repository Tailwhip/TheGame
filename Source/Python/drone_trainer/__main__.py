#!/usr/bin/env python

import signal
import sys
import time

from .uecommunication.ue_communication_server import UE5Server
from .uecommunication.message \
    import Message, msg_types, cmd_value_types, Signal
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
        time.sleep(5)
        clients = server.clients
        
        for i, id in enumerate(clients):
            client = clients[id]
            payload = [i, id]
            logger.info(f"Sending message ({payload}) to client: {id} / {client.address}")
            msg = Message(
                client_id = id,
                message_type = msg_types["Commands"],
                signals = [
                    Signal(cmd_value_types["s"], payload)
                ])
            server.send_data(client.address, msg)
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

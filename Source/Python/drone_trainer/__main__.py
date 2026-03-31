#!/usr/bin/env python

import signal
import sys
import time
import struct

from .uecommunication.ue_communication_server import UE5Server
from .uecommunication.message \
    import Message, msg_type, value_type, Signal
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
        
        while server.run:
            time.sleep(1)
            for i, id in enumerate(clients):
                client = clients[id]
                payload = []
                payload.extend(struct.pack("f", float(i) + 0.14))
                payload.extend(struct.pack("f", float(id) + 0.11))
                payload.extend(struct.pack("f", float(i) + 0.04))
                payload.extend(struct.pack("f", float(id) + 0.53))
                logger.info(f"Sending message ({payload}) to client: {id} / {client.address}")
                msg = Message(
                    client_id = id,
                    message_type = msg_type["Command"],
                    signal = Signal(
                        type=value_type["PAYLOAD"],
                        payload_layout_index=1,
                        value=payload))
                server.send_data(client.address, msg)
            signal_handler = SignalHandler(server.stop_server)
            signal.signal(signal.SIGINT, signal_handler)
        
    except Exception as e:
        logger.error(f"An error has occured: {e}")
    except KeyboardInterrupt:
        logger.error(f"Keyboard interrupted")
    finally:
        logger.info(f"Finally...")
                
    logger.info(f"Drone Trainer exit")


if __name__ == "__main__":
    main()

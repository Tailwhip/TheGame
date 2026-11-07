#!/usr/bin/env python

import signal
import sys
import time
import struct
from typing import Dict

from .uecommunication.ue_communication_server import UE5Server
from .uecommunication.message \
    import Message, msg_type, value_type, Signal, STEP_TIME_S
from .signal_handler import *
from .logger import logger


def debug_input(server: UE5Server, clients: Dict) -> None:
    time.sleep(STEP_TIME_S)
    for i, id in enumerate(clients):
        client = clients[id]
        payload = []
        payload.extend(struct.pack("f", float(1)))
        payload.extend(struct.pack("f", float(1)))
        payload.extend(struct.pack("f", float(id) + 0.04))
        payload.extend(struct.pack("f", float(id) - 0.53))
        logger.debug(
            f"Sending message ({payload}) to client: {id} / {client.address}")
        msg = Message(
            client_id = id,
            message_type = msg_type["Command"],
            signal = Signal(
                type=value_type["PAYLOAD"],
                payload_layout_index=1,
                value=payload))
        server.send_data(client.address, msg)

def main():
    logger.debug(f"Drone Trainer has started")
    server = None
    signal_handler = SignalHandler()
    signal.signal(signal.SIGINT, signal_handler)
    if hasattr(signal, "SIGBREAK"):  # Windows: Ctrl+Break
        signal.signal(signal.SIGBREAK, signal_handler)
    try:
        logger.debug(f"Starting UE communication server")
        server = UE5Server()
        server.start_server()
        time.sleep(5)
        clients = server.clients

        while server.run and not signal_handler.stop_requested:
            debug_input(server, clients)

    except Exception as e:
        logger.error(f"An error has occured: {e}")
    except KeyboardInterrupt:
        logger.error(f"Keyboard interrupted")
    finally:
        logger.debug(f"Finally...")
        if server is not None:
            server.stop_server()

    logger.debug(f"Drone Trainer exit")


if __name__ == "__main__":
    main()

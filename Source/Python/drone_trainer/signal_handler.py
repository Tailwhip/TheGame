#!/usr/bin/env python

import signal
import sys
import time

from .logger import logger


class SignalHandler(object):
    def __init__(self, handling_procedure):
        self.handling_procedure = handling_procedure

    def __call__(self, signal, frame):
        logger.info(f"Singal no. {signal} occured while" 
                    f" executing line {frame}. You pressed Ctrl+C!")
        self.handling_procedure()
        logger.info(f"Drone Trainer exit")
        sys.exit(0)

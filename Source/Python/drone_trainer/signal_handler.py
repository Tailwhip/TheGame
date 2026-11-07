#!/usr/bin/env python

import signal
import sys
import time

from .logger import logger


class SignalHandler(object):
    """Records that a stop was requested instead of shutting down inside
    the signal handler.

    Calling blocking cleanup code (joins, locks) from a signal handler runs
    it in the main thread, where it can interrupt that thread while it holds
    a lock and deadlock. The main loop polls ``stop_requested`` and performs
    the shutdown itself, at a safe point.
    """

    def __init__(self, handling_procedure=None):
        self.handling_procedure = handling_procedure
        self.stop_requested = False

    def __call__(self, signal, frame):
        logger.info(f"Signal no. {signal} occured. You pressed Ctrl+C!")
        self.stop_requested = True
        if self.handling_procedure is not None:
            self.handling_procedure()

import socket as st
import selectors
import os
import sys
import argparse
import threading
import traceback
from dataclasses import dataclass
from time import sleep

from ..logger import logger
from ..signal_handler import *


msg_types = {
    "None" : 0,
	"Register" : 1,
	"Snapshot" : 2,
	"Unregister" : 3,
    "Server_response" : 4,
    "Command" : 5
}

@dataclass(init=True, eq=True)
class Command:
    data: bytes = 0

    def serialize(self):
        return (self.data.len()).to_bytes(1, "big") + \
                self.data.to_bytes(1, "big")
            
            

@dataclass(init=True, eq=True)
class Message:
    client_id: int = 0
    message_type: int = 0
    commands: list[bytes] = 0
    
    def serialize(self):
        data = self.client_id.to_bytes(1, "big")
        data += self.message_type.to_bytes(1, "big")
        for c in self.commands:
            data += c.serialize()
        return 


import socket as st
import selectors
import os
import sys
import argparse
import threading
import traceback
import struct
from dataclasses import dataclass
from time import sleep
from typing import ClassVar

from ..logger import logger
from ..signal_handler import *


msg_types = {
    "None" : 0,
	"Register" : 1,     # [Id] [MsgType] [DataLen=1] [SignalDataType=1][SignalData=1/0]...
	"Snapshot" : 2,     # [Id] [MsgType] [DataLen]   [SignalDataType1][SignalData1]...
	"Unregister" : 3,   # [Id] [MsgType] [DataLen]   [SignalDataType1][SignalData1]...
    "Commands" : 4      # [Id] [MsgType] [DataLen]   [SignalDataType1][SignalData1]...
}

cmd_value_types = {
    "None" : 0,
	"B" : 1, # "UINT8"
	"H" : 2, # "UINT16"
	"I" : 3, # "UINT32"
	"Q" : 4, # "UINT64"
	"f" : 5, # "FLOAT"
	"d" : 6, # "DOUBLE"
	"s" : 7, # "PAYLOAD"
    "?" : 8  # "BOOL"
}


@dataclass(init=True, eq=True)
class Signal:
    type: int = cmd_value_types["None"]
    value: any = 0

    def serialize(self):
        return self.type.to_bytes(1, "big") + self.value.to_bytes(1, "big")
    
    def deserialize(self, type: int, value: bytes):
        self.type = type
        typeFormat = [t for t in cmd_value_types.keys if cmd_value_types[t] == type]
        self.value = struct.unpack(typeFormat[0], value)
        
        
@dataclass(eq=True)
class Message:
    client_id: int
    message_type: int
    signals: list[Signal]
       
    def __init__(self, client_id: int=0, message_type: int=0, 
                signals: list[Signal]=[Signal(0, 0x00)]):
        self.client_id = client_id
        self.message_type = message_type
        self.signals = signals
    
    def __hash__(self):
        return self.client_id
        
    def serialize(self):
        logger.info("Serializing...")
        data = self.client_id.to_bytes(1, "big")
        data += self.message_type.to_bytes(1, "big")
        data += len(self.signals).to_bytes(1, "big")
        for s in self.signals:
            data += s.serialize()
        return data
    
    def deserialize(self, payload):
        logger.info(f"Deserializing...{payload}")
        if len(payload) < 3:
            logger.error(f"Received message is too short!")
            return
        self.client_id = payload[0]
        self.message_type = payload[0]
        for i in range(2, len(payload), 2):
            logger.info(f"{payload[i]}")
            self.signals.append(Signal)
            
        
        


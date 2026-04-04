import socket as st
import selectors
import os
import sys
import argparse
import threading
import traceback
import struct
from typing import Any, Dict
from dataclasses import dataclass, field
from time import sleep
from typing import ClassVar

from ..logger import logger
from ..signal_handler import *


MAX_DATA_SIZE = 1024
STEP_TIME_S = 1

msg_type = {
    "None"       : 0,
	"Register"   : 1,   # [Id] [MsgType=1] [ValueType=1] [PayloadLayout=0] [SignalValue=1]...
	"Snapshot"   : 2,   # [Id] [MsgType]   [ValueType]   [PayloadLayout]   [SignalValue1]...
	"Unregister" : 3,   # [Id] [MsgType=3] [ValueType=1] [PayloadLayout]   [SignalValue1=1]...
    "Command"    : 4    # [Id] [MsgType]   [ValueType]   [PayloadLayout]   [SignalValue1]...
}

value_type = {
    "None"      : 0,
	"UINT8"     : 1,
	"UINT16"    : 2,
	"UINT32"    : 3, 
	"UINT64"    : 4, 
	"FLOAT"     : 5, 
	"DOUBLE"    : 6, 
	"PAYLOAD"   : 7,    # [SignalValue] as bytes payload: [[PayloadLayout], [SignalValueBytes]]
    "BOOL"      : 8
}

type_format = {
	1 : "B",
	2 : "H",
	3 : "I",
	4 : "Q",
	5 : "f",
	6 : "d",
	7 : "s",
    8 : "?"
}

# TODO: Maybe would be nice to move it to some common configuration file
payload_layout = {
    0 : [], # Use when the ValueType is not a payload
    1 : [   # Command layout: DroneContoller
        "FLOAT", "FLOAT",   # Movement
        "FLOAT", "FLOAT"    # Look
    ],
    2 : [   # Snapshot layout: DroneInfo
        "FLOAT", "FLOAT", "FLOAT",  # Position
        "FLOAT", "FLOAT", "FLOAT",  # Rotation
        "BOOL", # Did damage
        "BOOL", # Did kill
        "BOOL", # Was hit
        "BOOL", # Was killed
    ],
}

@dataclass()
class Signal:
    type: int = value_type["None"]
    payload_layout_index: int = 0
    value: Any = 0

    def serialize(self) -> bytes:
        result = self.type.to_bytes(1, "big")
        result += self.payload_layout_index.to_bytes(1, "big")
        if isinstance(self.value, list):
            result += bytearray(self.value)
        else:
            result += self.value.to_bytes(1, "big")
        return result
    
    def deserialize(self, payload: bytes, start_pos: int = 0) -> None:
        if len(payload) < (start_pos + 3):
            logger.error(f"Payload is too short!")
            return
        current_pos = start_pos
        self.type = payload[current_pos]
        current_pos += 1
        self.payload_layout_index = payload[current_pos]
        current_pos += 1
        logger.debug(
            f"Derialized Signal type: {self.type} /"
            f" payload_layout: {self.payload_layout_index} /"
            f" current position: {current_pos} /"
            f" payload: {payload}")
        if self.type == 0:
            logger.error(f"Type is None!")
            return
        if not self.type in type_format.keys():
            logger.error(f"Unknown Type: {self.type}!")
            return
        if self.type == 7:
            self.value = payload[current_pos:]
        else:
            self.value = struct.unpack(type_format[self.type], payload[current_pos:])

@dataclass()
class Message:
    client_id: int = 0
    message_type: int = 0
    signal: Signal = field(default_factory=Signal)

    def __hash__(self):
        return self.client_id
        
    def serialize(self):
        logger.debug("Serializing...")
        data = self.client_id.to_bytes(1, "big")
        data += self.message_type.to_bytes(1, "big")
        data += self.signal.serialize()
        return data
    
    def deserialize(self, payload: bytes, start_pos: int = 0):
        logger.debug(f"Deserializing payload: {payload}")
        current_pos = start_pos
        if len(payload) < 5:
            logger.error(f"Received message is too short!")
            return
        self.client_id = payload[current_pos]
        current_pos += 1
        self.message_type = payload[current_pos]
        current_pos += 1
        self.signal.deserialize(payload, current_pos)

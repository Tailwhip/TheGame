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


msg_type = {
    "None"       : 0,
	"Register"   : 1,   # [Id] [MsgType=1] [ValueType=1] [PayloadLayout=0] [SignalValue=1]...
	"Snapshot"   : 2,   # [Id] [MsgType]   [ValueType]   [PayloadLayout]   [SignalValue1]...
	"Unregister" : 3,   # [Id] [MsgType]   [ValueType]   [PayloadLayout]   [SignalValue1]...
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

payload_layout = {
    0 : [], # Use when the ValueType is not a payload
    1 : ["FLOAT", "FLOAT", "FLOAT", "FLOAT", "FLOAT", "FLOAT"]
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
        logger.info(
            f"Derialized Signal type: {self.type} / payload_layout: {self.payload_layout_index}")
        if self.type == 0:
            logger.error(f"Type is None!")
            return
        if not self.type in type_format.keys():
            logger.error(f"Unknown Type: {self.type}!")
            return
        self.value = struct.unpack(type_format[self.type], payload[current_pos:])

@dataclass()
class Message:
    client_id: int = 0
    message_type: int = 0
    signal: Signal = field(default_factory=Signal)

    def __hash__(self):
        return self.client_id
        
    def serialize(self):
        logger.info("Serializing...")
        data = self.client_id.to_bytes(1, "big")
        data += self.message_type.to_bytes(1, "big")
        data += self.signal.serialize()
        return data
    
    def deserialize(self, payload: bytes, start_pos: int = 0):
        logger.info(f"Deserializing payload: {payload}")
        current_pos = start_pos
        if len(payload) < 5:
            logger.error(f"Received message is too short!")
            return
        self.client_id = payload[current_pos]
        current_pos += 1
        self.message_type = payload[current_pos]
        current_pos += 1
        self.signal.deserialize(payload, current_pos)

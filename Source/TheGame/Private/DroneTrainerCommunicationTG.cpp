// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneTrainerCommunicationTG.h"

namespace DroneTrainerCommTG
{

TTuple<PayloadLen, const Byte*> Message::Serialize() const
{
	// uint8_t* bytes = {};
	Payload bytes;
	bytes.Append(reinterpret_cast<uint8*>(RegisterId), sizeof(RegId));
	bytes.Append(reinterpret_cast<uint8*>(MessageType), sizeof(MsgType));
	PayloadLen Len = Data.Num();
	bytes.Append(reinterpret_cast<uint8*>(Len), sizeof(PayloadLen));
	bytes.Append(Data.GetData(), Len * sizeof(Byte));
	return {bytes.Num(), bytes.GetData()};
}

	
	
}

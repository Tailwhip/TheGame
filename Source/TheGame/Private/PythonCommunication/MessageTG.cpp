// Fill out your copyright notice in the Description page of Project Settings.


#include "PythonCommunication/MessageTG.h"


FMessageTG::FMessageTG() :
	RegisterId{ 0 },
	Type{ MsgType::None }
{
	TRACE("Default Constructor")
}


FMessageTG::FMessageTG(RegId regId, MsgType msgType) :
	RegisterId(regId),
	Type(msgType)
{
	TRACE("Custom Constructor")
	if (MsgType::Register == Type)
	{
		TRACE("Preparing registration message with Reg Id: %d", RegisterId);
		DataSignal.SetValue<uint8>(1);
		DataSignal.Type = SignalValueType::UINT8;
	}
}

FMessageTG::FMessageTG(const FMessageTG& other) :
	RegisterId(other.RegisterId),
	Type(other.Type),
	DataSignal(other.DataSignal)
{
	TRACE("Copy constructor")
}

FMessageTG::FMessageTG(FMessageTG&& other) :
	RegisterId(std::move(other.RegisterId)),
	Type(std::move(other.Type)),
	DataSignal(MoveTemp(other.DataSignal))
{
	TRACE("Move constructor")
}

FMessageTG& FMessageTG::operator=(const FMessageTG& other)
{
	TRACE("Copy operator=")
	this->RegisterId = other.RegisterId;
	this->Type = other.Type;
	this->DataSignal = other.DataSignal;
	return *this;
}

FMessageTG& FMessageTG::operator=(FMessageTG&& other)
{
	TRACE("Move operator=")
	this->RegisterId = std::move(other.RegisterId);
	this->Type = std::move(other.Type);
	this->DataSignal = MoveTemp(other.DataSignal);
	return *this;
}

bool FMessageTG::Serialize(Payload& OutBuffer)
{
	TRACE("Reg Id: %d, Msg Type: %d", RegisterId, Type);
	OutBuffer.Empty();
	TRACE("RegisterId: %d", RegisterId);
	OutBuffer.Append(ToBytes(RegisterId), sizeof(RegId));
	OutBuffer.Append(ToBytes(Type), sizeof(MsgType));
	if ( !DataSignal.Serialize(OutBuffer) )
	{
		TRACEERROR("Failed to serialize signal data!");
		return false;
	}
	if (OutBuffer.IsEmpty()) return false;
	return true;
}

size_t FMessageTG::Deserialize(Payload& data, size_t startPos)
{
	TRACE("Deserializing %d bytes of data starting at position: %d", data.Num(), startPos);
	TRACEBYTES(data.GetData(), data.Num())
		if ((startPos + 2) > data.Num())
		{
			TRACEERROR("Read out of range of given data!");
			return startPos;
		}
	size_t currBufPos = startPos;

	FMemory::Memcpy(&RegisterId, data.GetData() + currBufPos, sizeof(RegId));
	currBufPos += sizeof(RegId);
	TRACE("RegisterId: %d Deserializing at position: %d", RegisterId, currBufPos);

	FMemory::Memcpy(&Type, data.GetData() + currBufPos, sizeof(MsgType));
	currBufPos += sizeof(MsgType);
	TRACE("Type: %d Deserializing at position: %d", Type, currBufPos);

	currBufPos = DataSignal.Deserialize(data, currBufPos);
	return currBufPos;
}

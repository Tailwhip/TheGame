// Fill out your copyright notice in the Description page of Project Settings.


#include "DroneTrainerCommunicationTG.h"
#include "Engine/World.h"


namespace DroneTrainerCommTG
{

Signal::Signal(const Signal& signal):
	Type(signal.Type),
	Value(signal.Value)
{
}

Signal::Signal(Signal&& signal):
	Type(std::move(signal.Type)),
	Value(std::move(signal.Value))
{
}
	
Signal& Signal::operator=(const Signal& signal)
{
	this->Type = signal.Type;
	this->Value = signal.Value;
	return *this;
}

Signal& Signal::operator=(Signal&& signal)
{
	this->Type = std::move(signal.Type);
	this->Value = std::move(signal.Value);
	return *this;
}

TTuple<PayloadLen, const Byte*> Signal::Serialize()
{
	TRACEWARN("");
	Bytes.Empty();
	Bytes.Append(ToBytes(Type), sizeof(uint8));
	const auto typeNum = static_cast<uint8>(Type);
	if ( (Value.index() != typeNum) || (Value.index() == 0) )
	{
		TRACEERROR("Wrong type! %lld / %d", Value.index(), typeNum);
	}
	else
	{
		if (std::holds_alternative<Payload>(Value))
		{
			Payload value = std::get<Payload>(Value);
			Bytes.Append(value.GetData(), value.Num());
		}
		else
		{
			std::visit([this](auto v){ Bytes.Append(ToBytes(v), sizeof(MsgType)); }, Value);
		}
	}
	return {Bytes.Num(), Bytes.GetData()};
}

uint32 Signal::Deserialize(const Byte* data)
{
	TRACEWARN("");
	uint32 currBufPos = 0;
	Byte typeBuf[sizeof(SignalValueType)] = {0};
	memcpy( typeBuf, data + currBufPos, sizeof(SignalValueType) );
	Type = static_cast<SignalValueType>(FromBytes<uint8>(typeBuf));
	currBufPos += sizeof(SignalValueType);
	const int32 typeLen = SignalValueTypeSizes[Type];
	Byte* valueBuf = (Byte*)calloc(1,typeLen);
	memcpy( valueBuf, data + currBufPos, typeLen );

	FString ReceivedDataHex;
	for (int32 i=0; i < sizeof(data); i++)
		ReceivedDataHex.Append(FString::Printf(TEXT("0x%02x "), data[i]));
	TRACEWARN("Data received (hex): %s", *ReceivedDataHex)
	
	switch (Type)
	{
		case SignalValueType::UINT8:
			Value = FromBytes<uint8>(valueBuf);
			break;
		case SignalValueType::UINT16:
			Value = FromBytes<uint16>(valueBuf);
			break;
		case SignalValueType::UINT32:
			Value = FromBytes<uint32>(valueBuf);
			break;
		case SignalValueType::UINT64:
			Value = FromBytes<uint64>(valueBuf);
			break;
		case SignalValueType::FLOAT:
			Value = FromBytes<float>(valueBuf);
			break;
		case SignalValueType::DOUBLE:
			Value = FromBytes<double>(valueBuf);
			break;
		case SignalValueType::PAYLOAD:
		{
			Value = Payload{valueBuf, typeLen};
			TRACEWARN("Signal value %s", (TCHAR*)valueBuf);
			break;
		}
		default:
			TRACEWARN("Unrecognized type: %d", static_cast<uint8>(Type));
	}
	
	Type = FromBytes<SignalValueType>(typeBuf);
	currBufPos += sizeof(SignalValueType);
	free(valueBuf);
	return currBufPos;
}

	
Message::Message(RegId regId, MsgType msgType, TArray<Signal>& data):
	RegisterId(regId),
	MessageType(msgType),
	Data(data)
{
}

TTuple<PayloadLen, const Byte*> Message::Serialize()
{
	TRACEWARN("Reg Id: %d, Msg Type: %d", RegisterId, MessageType);
	Bytes.Empty();
	Bytes.Append(ToBytes(RegisterId), sizeof(RegId));
	Bytes.Append(ToBytes(MessageType), sizeof(MsgType));
	PayloadLen Len = Data.Num();
	Bytes.Append(ToBytes(Len), sizeof(PayloadLen));
	for (auto Signal: Data)
	{
		TTuple<PayloadLen, const Byte*> s = Signal.Serialize();
		Bytes.Append(ToBytes(s.Value), sizeof(s.Key));
	}
	
	return {Bytes.Num(), Bytes.GetData()};
}

uint32 Message::Deserialize(const Byte* data)
{
	TRACEWARN("");
	uint32 currBufPos = 0;
	Byte regIdBuf[sizeof(RegId)] = {0};
	memcpy( regIdBuf , data + currBufPos , sizeof(RegId) );
	RegisterId = FromBytes<RegId>(regIdBuf);
	currBufPos += sizeof(RegId);
	Byte msgTypeBuf[sizeof(MsgType)] = {0};
	memcpy( msgTypeBuf , data + currBufPos, sizeof(MsgType) );
	MessageType = FromBytes<MsgType>(msgTypeBuf);
	currBufPos += sizeof(MsgType);
	Byte signalsNumBuf[sizeof(PayloadLen)] = {0};
	memcpy( signalsNumBuf , data + sizeof(RegId) + currBufPos, sizeof(PayloadLen) );
	PayloadLen sigNum = FromBytes<PayloadLen>(signalsNumBuf);
	currBufPos += sizeof(PayloadLen);
	for (PayloadLen i = 0; i < sigNum; i++)
	{
		const uint32 bufLen = (MAX_DATA_SIZE - currBufPos);
		Byte* sigBuf = (Byte*)calloc(bufLen,sizeof(Byte));
		memcpy( sigBuf , data + currBufPos, bufLen * sizeof(Byte) );
		Signal sig;
		currBufPos += sig.Deserialize(sigBuf);
		Data.Add(std::move(sig));
		free(sigBuf);
	}
	
	return currBufPos;
}

	
}

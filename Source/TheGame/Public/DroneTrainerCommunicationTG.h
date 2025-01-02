// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <variant>
#include <optional>
// #include "Misc/Variant.h
#include "GameModeBaseTG.h"
#include "TheGame/TheGame.h"
#include "CoreMinimal.h"

/**
 * 
 */

#define MAX_DATA_SIZE 1024 // bytes

namespace DroneTrainerCommTG
{
using RegId = uint8;
using Byte = uint8;
using Payload = TArray<Byte>;
using PayloadLen = uint8;
using SignalVariantType = std::variant<uint8, uint16, uint32, uint64, float, double, Payload>;
	
enum class MsgType: uint8
{
	None = 0,
	Register,		// [Id] [MsgType] [DataLen=1] [SignalDataType=1][SignalData=1/0]...
	Snapshot,		// [Id] [MsgType] [DataLen]   [SignalDataType1][SignalData1]...
	Unregister,		// [Id] [MsgType] [DataLen]   [SignalDataType1][SignalData1]...
	Command			// [Id] [MsgType] [DataLen]   [SignalDataType1][SignalData1]...
};

enum class SignalValueType: uint8
{
	None = 0,
	UINT8,
	UINT16,
	UINT32,
	UINT64,
	FLOAT,
	DOUBLE,
	PAYLOAD
};
	
template<typename T>
uint8* ToBytes(T& value)
{
	return reinterpret_cast<uint8*>(&value);
}

template<typename T>
T FromBytes(uint8* buffer)
{
	return static_cast<T>(*buffer);
}
	
struct Signal
{
	Signal() = default;
	Signal(const Signal& signal);
	Signal(Signal&& signal);
	Signal& operator=(const Signal& signal);
	Signal& operator=(Signal&& signal);
	
	SignalValueType Type{SignalValueType::None};
	SignalVariantType Value{};
	
	TTuple<PayloadLen, const Byte*> Serialize();
	uint32 Deserialize(const Byte* data);

private:
	Payload Bytes;

	std::map<SignalValueType, uint8> SignalValueTypeSizes =
	{
		{SignalValueType::UINT8, 8},
		{SignalValueType::UINT16, 16},
		{SignalValueType::UINT32, 32},
		{SignalValueType::UINT64, 64},
		{SignalValueType::FLOAT, 32}, // TODO: Confirm the size
		{SignalValueType::DOUBLE, 64} // TODO: Confirm the size
	};
	
};
	
struct Message
{
	Message(RegId regId, MsgType msgType, TArray<Signal>& data);
	
	RegId RegisterId{0};
	MsgType MessageType{MsgType::None};
	TArray<Signal>& Data;
	
	TTuple<PayloadLen, const Byte*> Serialize();
	uint32 Deserialize(const Byte* data);
private:
	Payload Bytes;
};

}

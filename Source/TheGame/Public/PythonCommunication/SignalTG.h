// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include <variant>
#include <optional>
#include "TheGame/TheGame.h"
#include "CoreMinimal.h"
#include "SignalTG.generated.h"


#define MIN_MSG_SIZE 5

using RegId = uint8;
using Byte = uint8;
using Payload = TArray<Byte>;
using PayloadLen = uint32;
using SignalVariantType = std::variant<
	std::monostate,
	uint8,
	uint16,
	uint32,
	uint64,
	float,
	double,
	Payload>;

enum class MsgType : uint8
{
	None = 0,
	Register,		// [Id] [MsgType] [ValueType=1] [PayloadLayout=0] [SignalValue=1]...
	Snapshot,		// [Id] [MsgType] [ValueType]   [PayloadLayout]   [SignalValue1]...
	Unregister,		// [Id] [MsgType] [ValueType]   [PayloadLayout]   [SignalValue1]...
	Command			// [Id] [MsgType] [ValueType]   [PayloadLayout]   [SignalValue1]...
};

enum class SignalValueType : uint8
{
	None = 0,
	UINT8,
	UINT16,
	UINT32,
	UINT64,
	FLOAT,
	DOUBLE,
	PAYLOAD // [SignalValue] as bytes payload: [[PayloadLayout], [SignalValueBytes]]
};

enum class PayloadLayoutType : uint8
{
	None = 0,
	DroneContoller,
	DroneInfo
};

template<typename T>
uint8* ToBytes(T& value)
{
	return reinterpret_cast<uint8*>(&value);
}

template<typename T>
T FromBytes(Payload& Data, size_t Offset)
{
	T temp;
	FMemory::Memcpy(&temp, Data.GetData() + Offset, sizeof(T));
	return temp;
}

/**
 * 
 */
USTRUCT()
struct THEGAME_API FSignalTG
{
	GENERATED_BODY()

	FSignalTG();
	~FSignalTG() = default;
	FSignalTG(const FSignalTG& other);
	FSignalTG(FSignalTG&& other);
	FSignalTG& operator=(const FSignalTG& other);
	FSignalTG& operator=(FSignalTG&& other);

	SignalValueType Type;
	SignalVariantType Value;
	PayloadLayoutType PayloadLayout;

	template<typename T>
	void SetValue(const T& value)
	{
		Value.emplace<T>(value);
	}

	template<typename T>
	void SetValue(T& value)
	{
		Value.emplace<T>(value);
	}

	template<typename T>
	T* GetValue()
	{
		if (Value.valueless_by_exception())
		{
			TRACEERROR("Signal Value is empty!")
			return nullptr;
		}
		return std::get_if<T>(&Value);
	}
	bool IsEmpty()
	{
		return std::holds_alternative<std::monostate>(Value);
	}
	bool Serialize(Payload& OutBuffer);
	size_t Deserialize(Payload& data, size_t startPos = 0);
};

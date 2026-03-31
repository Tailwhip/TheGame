// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PythonCommunication/SignalTG.h"
#include "MessageTG.generated.h"

/**
 * 
 */
USTRUCT()
struct THEGAME_API FMessageTG
{
	GENERATED_BODY()

	FMessageTG();
	~FMessageTG() = default;
	FMessageTG(RegId regId, MsgType msgType);
	FMessageTG(const FMessageTG& other);
	FMessageTG(FMessageTG&& other);
	FMessageTG& operator=(const FMessageTG& other);
	FMessageTG& operator=(FMessageTG&& other);

	RegId RegisterId;
	MsgType Type;
	FSignalTG DataSignal;

	bool Serialize(Payload& OutBuffer);
	size_t Deserialize(Payload& data, size_t startPos = 0);
};
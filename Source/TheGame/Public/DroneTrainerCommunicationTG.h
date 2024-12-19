// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
namespace DroneTrainerCommTG
{
using RegId = uint8;
using Byte = uint8;
using Payload = TArray<Byte>;
using PayloadLen = uint8;

enum class MsgType: uint8
{
	None = 0,
	Register,
	Snapshot,
	Unregister
};
	
struct THEGAME_API Message
{
	RegId RegisterId{0};
	MsgType MessageType{MsgType::None};
	Payload Data = TArray<Byte>{0x00};

	TTuple<PayloadLen, const Byte*> Serialize() const;
};

}

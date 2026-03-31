// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class THEGAME_API ICommandTG
{
public:
	ICommandTG() = default;
	virtual ~ICommandTG() = default;
};

class THEGAME_API CommandTG : public ICommandTG
{
public:
	CommandTG();
	~CommandTG();
};

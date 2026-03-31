// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class THEGAME_API ISnapshotTG
{
public:
	ISnapshotTG() = default;
	virtual ~ISnapshotTG() = default;
};

class THEGAME_API SnapshotTG : public ISnapshotTG
{
public:
	SnapshotTG();
	~SnapshotTG();
};

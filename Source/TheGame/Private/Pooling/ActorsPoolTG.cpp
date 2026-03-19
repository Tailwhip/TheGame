// Fill out your copyright notice in the Description page of Project Settings.


#include "Pooling/ActorsPoolTG.h"
#include "Base/ActorTG.h"
#include "TheGame/TheGame.h"


FActorsPoolTG::FActorsPoolTG(SizeType Size):
	CurrentNum{ 0 }
{
	TRACE("Creating Pool with size: %d", Size)
	Pool.Reserve(Size);
}

AActorTG* FActorsPoolTG::GetNext()
{
	uint32 pos = CurrentNum;
	TRACE("Getting next element from the pool. Current num: %d", CurrentNum)
	if (Pool.Num() == 0)
	{
		TRACEERROR("Pool is empty!")
		return nullptr;
	}
	auto CurrentObj = Pool[pos];
	if (CurrentObj->IsInUse())
	{
		TRACEERROR("All elements are currently in use!")
		return nullptr;
	}
	CurrentNum++;
	CurrentNum = CurrentNum % Pool.Num();
	return Pool[pos];
}

void FActorsPoolTG::Add(AActorTG* NewActor)
{
	TRACE("Add new element to the pool")
	Pool.Add(NewActor);
}

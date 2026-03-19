// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Containers/Array.h>
#include "ActorsPoolTG.generated.h"

class AActorTG;

/**
 * 
 */
USTRUCT(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
struct THEGAME_API FActorsPoolTG
{
	GENERATED_BODY()

	using SizeType = size_t;

	FActorsPoolTG() = default;
	~FActorsPoolTG() = default;
	FActorsPoolTG(SizeType Size);

	void Add(AActorTG* NewActor);
	AActorTG* GetNext();
	SizeType Size() const { return Pool.Num(); }
private:
	UPROPERTY()
	TArray<AActorTG*> Pool;

	UPROPERTY()
	uint32 CurrentNum;
};

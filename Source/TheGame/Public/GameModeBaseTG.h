// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeBaseTG.generated.h"

/**
 * 
 */
UCLASS()
class THEGAME_API AGameModeBaseTG : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameModeBaseTG();
	~AGameModeBaseTG() = default;

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

// private:
	// UPROPERTY()
	// TWeakObjectPtr<UMLAdapterLocDataCollectSessionTG> PlayerControlledAgent;
};

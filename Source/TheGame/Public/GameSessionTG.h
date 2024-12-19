// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "MLAdapterLocDataCollectSessionTG.h"
#include "GameSessionTG.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class THEGAME_API AGameSessionTG : public AGameSession
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void AddAgent();
	
// private:
// 	UPROPERTY()
// 	TWeakObjectPtr<UMLAdapterLocDataCollectSessionTG> MLAdapterDataCollectSession;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DroneHudTG.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class THEGAME_API UDroneHudTG : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetHealth(float CurrentHealth, float MaxHealth);

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	class UProgressBar* HealthBar;
};

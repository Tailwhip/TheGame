// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterHUDWidgetTG.generated.h"


/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class THEGAME_API UCharacterHUDWidgetTG : public UUserWidget
{
	GENERATED_BODY()

public:
    // Called to update values
    UFUNCTION(BlueprintCallable)
    void SetHealth(float CurrentHealth, float MaxHealth);

    UFUNCTION(BlueprintCallable)
    void SetAmmo(int32 CurrentAmmo, int32 MaxAmmo);

protected:
    // Bind these in Blueprint using "BindWidget"
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* AmmoText;
};

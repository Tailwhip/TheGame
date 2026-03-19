// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CharacterHUDWidgetTG.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"



void UCharacterHUDWidgetTG::SetHealth(float CurrentHealth, float MaxHealth)
{
    if (HealthBar && MaxHealth > 0.0f)
    {
        float Percent = CurrentHealth / MaxHealth;
        HealthBar->SetPercent(Percent);
    }
}

void UCharacterHUDWidgetTG::SetAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
    if (AmmoText)
    {
        FString AmmoString = FString::Printf(TEXT("%d / %d"), CurrentAmmo, MaxAmmo);
        AmmoText->SetText(FText::FromString(AmmoString));
    }
}
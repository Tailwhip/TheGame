// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DroneHudTG.h"

#include "Components/ProgressBar.h"

void UDroneHudTG::SetHealth(float CurrentHealth, float MaxHealth)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(CurrentHealth / MaxHealth);
	}
}

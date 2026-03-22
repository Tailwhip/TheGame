// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputActionValue.h"
#include "CoreMinimal.h"

/**
 * 
 */
class THEGAME_API FControllerTG
{
public:
	FControllerTG();
	virtual ~FControllerTG();

	virtual void HandleMove(const FInputActionValue& InputActionValue);
	virtual void HandleLook(const FInputActionValue& InputActionValue) = 0;
	virtual void HandleJump();
	virtual void HandleCrouch();
	virtual void HandleAscend(const FInputActionValue& InputActionValue);
	virtual void HandleDescend(const FInputActionValue& InputActionValue);
	virtual void HandleShooting();
protected:
	// Used to store a reference to the pawn we are controlling.
	UPROPERTY()
	TObjectPtr<class ACharacterTG> ControlledCharacter{ nullptr };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "ControllerTG")
	float Sensitivity{ 1.3f };
};

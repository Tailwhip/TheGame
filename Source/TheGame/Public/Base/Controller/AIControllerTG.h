// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Base/Controller/ControllerTG.h"
#include "AIControllerTG.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class THEGAME_API AAIControllerTG : 
	public AAIController,
	public FControllerTG
{
	GENERATED_BODY()

public:
	AAIControllerTG();
	~AAIControllerTG();

protected:
	void HandleLook(const FInputActionValue& InputActionValue) override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

private:
	UPROPERTY()
	class UMessageDispatcherTG* MessageDispacher;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"

#include "Base/Controller/ControllerTG.h"
#include "PythonCommunication/SignalTG.h"
#include "AIControllerTG.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class THEGAME_API AAIControllerTG : 
	public AAIController,
	public FControllerTG
{
	GENERATED_BODY()

public:
	AAIControllerTG();
	~AAIControllerTG();

	RegId RegisterId;
protected:
	void HandleLook(const FInputActionValue& InputActionValue) override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

private:
	bool RegisterDelegates();
	void SendSnapshot();

	bool bDelegatesRegistered;
	// TODO: Move to Tick method
	FTimerDelegate TimerDelegate;
	FTimerHandle TickTimerHandle;
	FDelegateHandle MoveDelegateHandle;
	FDelegateHandle LookDelegateHandle;

	UPROPERTY()
	class UPythonCommunicationComponentTG* CommunicationComponent;

	UPROPERTY()
	class UMessageDispatcherTG* MessageDispacher;
};

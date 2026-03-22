// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DroneTrainerCommunicationTG.h"
#include "PythonCommunicationComponentTG.generated.h"


UCLASS( BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent) )
class THEGAME_API UPythonCommunicationComponentTG : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPythonCommunicationComponentTG();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadWrite, Category = "PythonCommunicationComponentTG")
	FString ReceivedData;

	UFUNCTION(BlueprintCallable, Category = "PythonCommunicationComponentTG")
	void OnReceivedDataChanged();

	UFUNCTION(BlueprintCallable, Category = "PythonCommunicationComponentTG")
	void PauseHandlingData();

	UFUNCTION(BlueprintCallable, Category = "PythonCommunicationComponentTG")
	void ResumeHandlingData();

	UPROPERTY(EditAnywhere, Category = "PythonCommunicationComponentTG")
	int32 ServerPort;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	bool HandleData();
	bool HandleConnection();
	bool SendMessage(DroneTrainerCommTG::Message& Message);
	bool ConnectToServer();
	void EndConnection();
	bool ReceiveMessage(DroneTrainerCommTG::Message& MessageContainer);

	FSocket* ClientSocket;
	// FSocket* ReceivingSocket;
	// Initializes a timer to periodically handle incoming data from connected clients (Python script).
	FTimerDelegate TimerDelegate;
	FTimerHandle TickTimerHandle;
	TArray<DroneTrainerCommTG::Signal> CurrentSignalsBuffer;
	
	static DroneTrainerCommTG::RegId CurrRegisterId;
	DroneTrainerCommTG::RegId RegisterId;
	bool bShouldHandleData;
	bool bShouldSendData;
	bool bIsRegistered;
};

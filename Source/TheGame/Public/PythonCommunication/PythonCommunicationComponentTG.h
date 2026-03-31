// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PythonCommunication/MessageTG.h"
#include "PythonCommunicationComponentTG.generated.h"


UCLASS( BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent) )
class THEGAME_API UPythonCommunicationComponentTG : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPythonCommunicationComponentTG();
	~UPythonCommunicationComponentTG();
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

	UPROPERTY()
	class UMessageDispatcherTG* MessageDispacher;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	bool HandleData(FMessageTG&& Message);
	bool HandleConnection(FMessageTG&& Message);
	bool SendMessage(FMessageTG& Message);
	bool ConnectToServer();
	void EndConnection();
	bool ReceiveMessage();

	FSocket* ClientSocket;
	// FSocket* ReceivingSocket;
	// Initializes a timer to periodically handle incoming data from connected clients (Python script).
	FTimerDelegate TimerDelegate;
	FTimerHandle TickTimerHandle;
	TArray<FSignalTG> CurrentSignalsBuffer;
	static RegId CurrRegisterId;
	RegId RegisterId;
	// Note: Data handling may be paused by Unreal Engine using bShouldHandleData flag.
	bool bShouldHandleData;
	bool bShouldSendData;
	bool bIsRegistered;
};

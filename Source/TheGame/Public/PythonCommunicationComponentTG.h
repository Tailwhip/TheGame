// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// #include "Python.h"
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

	UPROPERTY(BlueprintReadWrite)
	FString ReceivedData;

	UFUNCTION(BlueprintCallable)
	void OnReceivedDataChanged();

	UFUNCTION(BlueprintCallable)
	void PauseHandlingData();

	UFUNCTION(BlueprintCallable)
	void ResumeHandlingData();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	void StartServer();
	void EndServer();
	void HandleData();
	void HandleConnection();
	void SendMessage(const DroneTrainerCommTG::Message& Message);

	void ConnectToServer();
	void EndConnection();

	FSocket* ReceivingSocket;
	FSocket* SendingSocket;
	FTimerHandle TickTimerHandle;

	UPROPERTY(EditAnywhere)
	int32 ReceivingPort = 7777;

	UPROPERTY(EditAnywhere)
	int32 SendingPort = 7776;

	static DroneTrainerCommTG::RegId CurrRegisterId;
	bool bShouldHandleData = true;
	bool bShouldSendData = false;
	
};

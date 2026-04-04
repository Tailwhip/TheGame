// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Containers/Queue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "PythonCommunication/MessageTG.h"
#include "MessageDispatcherTG.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(
	FOnDroneMovementReceived, const FInputActionValue&);
DECLARE_MULTICAST_DELEGATE_OneParam(
	FOnDroneLookReceived, const FInputActionValue&);
/**
 * 
 */
UCLASS()
class THEGAME_API UMessageDispatcherTG : public UObject
{
	GENERATED_BODY()

public:
	UMessageDispatcherTG();
	~UMessageDispatcherTG();

	// Commands Delegates
	FOnDroneMovementReceived OnDroneMovementReceived;
	FOnDroneLookReceived OnDroneLookReceived;
	// Snapshot Delegates

	void AddReceivedMessage(FMessageTG&& ReceivedMsg);
	void AddMessageToSend(FMessageTG&& ToSendMsg);

	void ProcessMessagesReceived();
	bool GetMessageToSend(FMessageTG& Message);

	void ComposeSnapshotMessage(
		RegId RegisterId,
		FVector& Location,
		FRotator& Rotation,
		bool bDidDamage,
		bool bDidKill,
		bool bWasHit,
		bool bWasKilled);
private:
	TQueue<FMessageTG> MessagesReceived;
	TQueue<FMessageTG> MessagesToSend;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "PythonCommunication/MessageDispatcherTG.h"

#include "TheGame/TheGame.h"


UMessageDispatcherTG::UMessageDispatcherTG()
{
	TRACE("Default Constructor")
}

UMessageDispatcherTG::~UMessageDispatcherTG()
{
	TRACE("Destructing...")
}

void UMessageDispatcherTG::AddReceivedMessage(FMessageTG&& ReceivedMsg)
{
	TRACE("")
	if (MsgType::Command == ReceivedMsg.Type)
	{
		TRACE("Add new command for id: %d", ReceivedMsg.RegisterId)
		MessagesReceived.Enqueue(MoveTemp(ReceivedMsg));
	}
	else
		TRACEERROR("Message received must be a Command type!")
}

void UMessageDispatcherTG::AddMessageToSend(FMessageTG&& ToSendMsg)
{
	TRACE("")
	if (MsgType::Snapshot == ToSendMsg.Type)
	{
		TRACE("Add new snapshot from id: %d", ToSendMsg.RegisterId)
		MessagesToSend.Enqueue(MoveTemp(ToSendMsg));
	}
	else
		TRACEERROR("Message to send must be a Snapshot type!")
}

void UMessageDispatcherTG::ProcessMessagesReceived()
{
	TRACE("")
	FMessageTG Message;
	while (MessagesReceived.Dequeue(Message))
	{
		TRACE("Processing next command...")
		PayloadLayoutType layoutType = Message.DataSignal.PayloadLayout;
		switch (layoutType)
		{
		case PayloadLayoutType::DroneContoller:
		{
			Payload* data = Message.DataSignal.GetValue<Payload>();
			if (!data)
			{
				TRACEERROR("Data is missing!")
				break;
			}
			size_t currPosition = 0;
			FVector DroneMovement;
			DroneMovement.X = FromBytes<float>(*data, currPosition);
			currPosition += sizeof(float);
			DroneMovement.Y = FromBytes<float>(*data, currPosition);
			TRACE("Broadcast movement X: %f / Y: %f", DroneMovement.X, DroneMovement.Y)
			OnDroneMovementReceived.Broadcast(FInputActionValue(DroneMovement));
			FVector DroneLook;
			currPosition += sizeof(float);
			DroneLook.X = FromBytes<float>(*data, currPosition);
			currPosition += sizeof(float);
			DroneLook.Y = FromBytes<float>(*data, currPosition);
			TRACE("Broadcast look X: %f / Y: %f", DroneLook.X, DroneLook.Y)
			OnDroneLookReceived.Broadcast(FInputActionValue(DroneLook));
		}
			break;
		default:
			TRACEWARN("Unrecognized payload layout type: %d", static_cast<uint8>(layoutType));
		}
	}
}

bool UMessageDispatcherTG::GetMessageToSend(FMessageTG& OutMessage)
{
	TRACE("");
	return MessagesToSend.Dequeue(OutMessage);
}

void UMessageDispatcherTG::ComposeSnapshotMessage(
	RegId RegisterId,
	FVector& Location,
	FRotator& Rotation,
	bool bDidDamage,
	bool bDidKill,
	bool bWasHit,
	bool bWasKilled)
{
	TRACE("");
	FMessageTG message;

	Payload payload;
	float x = Location.X;
	payload.Append(ToBytes(x), sizeof(float));
	float y = Location.Y;
	payload.Append(ToBytes(y), sizeof(float));
	float z = Location.Z;
	payload.Append(ToBytes(z), sizeof(float));

	float pitch = Rotation.Pitch;
	payload.Append(ToBytes(pitch), sizeof(float));
	float yaw = Rotation.Yaw;
	payload.Append(ToBytes(yaw), sizeof(float));
	float roll = Rotation.Roll;
	payload.Append(ToBytes(roll), sizeof(float));

	payload.Append(ToBytes(bDidDamage), sizeof(bool));
	payload.Append(ToBytes(bDidKill), sizeof(bool));
	payload.Append(ToBytes(bWasHit), sizeof(bool));
	payload.Append(ToBytes(bWasKilled), sizeof(bool));

	message.RegisterId = RegisterId;
	message.Type = MsgType::Snapshot;
	FSignalTG signal;
	signal.Type = SignalValueType::PAYLOAD;
	signal.PayloadLayout = PayloadLayoutType::DroneInfo;
	signal.SetValue<Payload>(payload);
	message.DataSignal = signal;

	AddMessageToSend(MoveTemp(message));
}



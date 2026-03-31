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

// Fill out your copyright notice in the Description page of Project Settings.

#include "PythonCommunicationComponentTG.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Networking.h"
#include "Sockets.h"
#include "Misc/ByteSwap.h"

#include "TheGame/TheGame.h"


uint8_t UPythonCommunicationComponentTG::CurrRegisterId{ 0 };

// Sets default values for this component's properties
UPythonCommunicationComponentTG::UPythonCommunicationComponentTG() :
	Super(),
	ServerPort{ 7777 },
	RegisterId { CurrRegisterId },
	bShouldHandleData{ true },
	bShouldSendData{ false },
	bIsRegistered{ false }
{
	TRACE("");
	CurrRegisterId++;
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UPythonCommunicationComponentTG::BeginPlay()
{
	TRACE("");
	Super::BeginPlay();
	ConnectToServer();
}

void UPythonCommunicationComponentTG::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TRACE("");
	EndConnection();
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void UPythonCommunicationComponentTG::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	TRACE("");
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// SendMessage("A client's message");
	// ...
}

void UPythonCommunicationComponentTG::OnReceivedDataChanged()
{
	TRACE("");
}

void UPythonCommunicationComponentTG::PauseHandlingData()
{
	TRACE("");
	bShouldHandleData = false;
}

void UPythonCommunicationComponentTG::ResumeHandlingData()
{
	TRACE("");
	bShouldHandleData = true;
}

bool UPythonCommunicationComponentTG::ConnectToServer()
{
	auto Owner = GetOwner();
	if (Owner)
		TRACE("%s is connecting to python server with reg id: %d", 
			*GetOwner()->GetName(), RegisterId)
	else
		TRACE("Unknown owner is connecting to python server with reg id: %d", RegisterId)
	// Sets up a TCP client connection with server on localhost:PORT.
	FIPv4Endpoint ListenerEndpoint(FIPv4Address(127, 0, 0, 1), ServerPort);
	// Listening for incoming connections
	ClientSocket = FUdpSocketBuilder(TEXT("Python Client"))
		.AsReusable(); // Can be used after being deleted (after the server is closed), without having to wait some time
	// .BoundToEndpoint(ListenerEndpoint) // Binds it to the Endpoint
	// .Listening(8); // Max number of pending connections
	
	if (!ClientSocket->Connect(*ListenerEndpoint.ToInternetAddrIPV4()))
	{
		TRACEERROR("The connection to server has failed!");
		return false;
	}
	
	if (ClientSocket)
	{
		TimerDelegate.BindLambda(
			[this]()
			{
				if (bIsRegistered)
					HandleData();
				else
					HandleConnection();
			});
		GetOwner()->GetWorldTimerManager().SetTimer(TickTimerHandle, TimerDelegate, 1.0f, true);
	}
	// Note: The server listens for incoming connections and uses a separate socket for sending responses to Python.
	return true;
}

// Send a response back to Python on a specified port.
bool UPythonCommunicationComponentTG::SendMessage(
	DroneTrainerCommTG::Message& Message) 
{
	TRACE("%s (Reg Id: %d) is sending message", *GetOwner()->GetName(), RegisterId);
	bool result = false;

	if (!ClientSocket)
	{
		TRACEERROR("ClientSocket is invalid!");
		return result;
	}

	// Sending the response data.
	TTuple<DroneTrainerCommTG::PayloadLen,
			const DroneTrainerCommTG::Byte*> MessageBuffer = Message.Serialize();
	const uint8* Data = MessageBuffer.Value;
	int32 PayloadLen = MessageBuffer.Key;
	int32 BytesSent = 0;

	FString DataHex;
	for (int32 i = 0; i < PayloadLen; ++i)
		DataHex.Append(FString::Printf(TEXT("0x%02x "), Data[i]));
	TRACE("Data to be sent: (hex): %s", *DataHex)
	result = ClientSocket->Send(Data, PayloadLen, BytesSent);
	if (BytesSent == PayloadLen)
		TRACE("Data has been sent successfully")
	else
	{
		TRACEERROR("BytesSent is not equal to Payload length!")
		result = false;
	}

	return result;
}

// Handles the incoming data from connected clients (Python script)
bool UPythonCommunicationComponentTG::HandleData()
{
	TRACE("")
	bool result = false;
	// Note: Data handling may be paused by Unreal Engine using bShouldHandleData flag.
	if (!bShouldHandleData)
	{
		TRACEWARN("UE System is currently busy");
		return result;
	}

	if (!ClientSocket)
	{
		TRACEERROR("Listener server stopped!");
		return result;
	}

	DroneTrainerCommTG::Message msg =
		DroneTrainerCommTG::Message(0, DroneTrainerCommTG::MsgType::None);
	result = ReceiveMessage(msg);
	return result;
}

bool UPythonCommunicationComponentTG::HandleConnection()
{
	TRACE("%s is connecting to the server", *GetOwner()->GetName(), RegisterId)
	bool result = false;

	if (!bShouldHandleData)
	{
		TRACEWARN("UE System is currently busy");
		return false;
	}

	DroneTrainerCommTG::Message registerMsg = DroneTrainerCommTG::Message(
		RegisterId,
		DroneTrainerCommTG::MsgType::Register);
	SendMessage(registerMsg);

	DroneTrainerCommTG::Message msg =
		DroneTrainerCommTG::Message(0, DroneTrainerCommTG::MsgType::None);
	if (ReceiveMessage(msg))
	{
		//uint8 value = std::get<uint8>(msg.Data[0].Value);
		uint8* valuePtr = msg.DataSignals[0].GetValue<uint8>();
		if (!valuePtr)
		{
			TRACEERROR("Received Value is empty!");
			return false;
		}
		if (DroneTrainerCommTG::MsgType::Register == msg.MessageType && 1 == *valuePtr)
		{
			bIsRegistered = true;
			TRACE("Client %s (regId: %d) registered successfully!", *GetOwner()->GetName(), RegisterId)
			result = true;
		}
	}
	return result;
}

// Closes the server socket and cleans up associated resources.
// Also closes the socket used for sending responses to Python.
void UPythonCommunicationComponentTG::EndConnection()
{
	TRACE("");
	if (ClientSocket)
	{
		ClientSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ClientSocket);
		ClientSocket = nullptr;
		UE_LOG(LogTemp, Log, TEXT("Sending socket ended."));
	}

	// if (ReceivingSocket)
	// {
	// 	ReceivingSocket->Close();
	// 	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ReceivingSocket);
	// 	ReceivingSocket = nullptr;
	// 	UE_LOG(LogTemp, Log, TEXT("Receiving socket ended"));
	// }
}

bool UPythonCommunicationComponentTG::ReceiveMessage(
	DroneTrainerCommTG::Message& MessageContainer)
{
	bool result = false;
	if (!ClientSocket)
	{
		TRACEERROR("Listener server stopped!");
		return false;
	}

	if (ClientSocket->GetConnectionState() == SCS_Connected)
	{
		uint32 pendingSize = 0;
		if (!ClientSocket->HasPendingData(pendingSize))
		{
			TRACEERROR("No pending messages found!")
			return false;
		}
		TRACE("%d bytes of payload waits to be read", pendingSize)

		int32 bytesRead = 0;
		TArray<uint8> payload;
		payload.SetNumUninitialized(pendingSize);

		TSharedRef<FInternetAddr> sender =
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

		if (!ClientSocket->RecvFrom(
			payload.GetData(),
			payload.Num(),
			bytesRead,
			*sender
		))
		{
			TRACEERROR("No data has been received!")
			return false;
		}
		else
		{
			TRACE("%d bytes of data have been read", bytesRead)
		}

		FString receivedDataHex;
		// Converts the data to FString and triggers an event (inside of Unreal Engine)
		for (auto ch : payload)
		{
			receivedDataHex.Append(FString::Printf(TEXT("0x%02x "), ch));
		}
		TRACE("Data received (hex): %s", *receivedDataHex)

		MessageContainer.Deserialize(payload.GetData());
		if (MessageContainer.DataSignals.IsEmpty())
		{
			TRACE("Received Message Data is empty!")
			return false;
		}
		result = true;
	}
	else
	{
		TRACEERROR("Client socket is not connected!");
		return false;
	}
	return result;
}

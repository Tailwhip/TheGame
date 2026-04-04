// Fill out your copyright notice in the Description page of Project Settings.

#include "PythonCommunicationComponentTG.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Networking.h"
#include "Sockets.h"
#include "Misc/ByteSwap.h"

#include "TheGame/TheGame.h"
#include "PythonCommunication/MessageDispatcherTG.h"


uint8_t UPythonCommunicationComponentTG::CurrRegisterId{ 0 };

TMap<SignalValueType, uint8> UPythonCommunicationComponentTG::SignalValueTypeSizes =
{
	{ SignalValueType::UINT8, 1 },
	{ SignalValueType::UINT16, 2 },
	{ SignalValueType::UINT32, 4 },
	{ SignalValueType::UINT64, 8 },
	{ SignalValueType::FLOAT, 4 }, // TODO: Confirm the size
	{ SignalValueType::DOUBLE, 8 } // TODO: Confirm the size
};

TMap<uint8, TArray<SignalValueType>> UPythonCommunicationComponentTG::PayloadLayouts =
{
	{ 0, {} },
	{ 1, {
		SignalValueType::FLOAT,
		SignalValueType::FLOAT,
		SignalValueType::FLOAT,
		SignalValueType::FLOAT,
		SignalValueType::FLOAT,
		SignalValueType::FLOAT} }
};

// Sets default values for this component's properties
UPythonCommunicationComponentTG::UPythonCommunicationComponentTG() :
	Super(),
	ServerPort{ 7777 },
	bIsRegistered{ false },
	RegisterId{ CurrRegisterId },
	bShouldHandleData{ true },
	bShouldSendData{ false }
{
	TRACE("");
	CurrRegisterId++;
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	// ...
}

UPythonCommunicationComponentTG::~UPythonCommunicationComponentTG()
{
	EndConnection();
}

// Called when the game starts
void UPythonCommunicationComponentTG::BeginPlay()
{
	TRACE("");
	Super::BeginPlay();
	MessageDispacher = NewObject<UMessageDispatcherTG>();
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

bool UPythonCommunicationComponentTG::IsConnected()
{
	if (!ClientSocket) return false;
	if (ClientSocket->GetConnectionState() != SCS_Connected) return false;
	return true;
}

bool UPythonCommunicationComponentTG::ConnectToServer()
{
	auto Owner = GetOwner();
	if (Owner)
		TRACE("%s is connecting to python server with reg id: %d", 
			*GetOwner()->GetName(), RegisterId)
	else
		TRACE("Unknown owner is connecting to python server with reg id: %d", 
			RegisterId)
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
		TimerDelegate.BindLambda([this]()
		{
			if (!bIsRegistered)
			{
				TRACE("%s is trying to register with id: %d", *GetOwner()->GetName(), RegisterId)
				FMessageTG registerMsg = FMessageTG(RegisterId, MsgType::Register);
				SendMessage(registerMsg);
			}
			else
			{
				FMessageTG msgToSend;
				while (MessageDispacher->GetMessageToSend(msgToSend))
				{
					TRACE("%s is sending new snapshot with id: %d", 
						*GetOwner()->GetName(), RegisterId)
					SendMessage(msgToSend);
				}
			}
			ReceiveMessage();
			if (MessageDispacher)
			{
				MessageDispacher->ProcessMessagesReceived(); //TODO: Move to TickComponent
			}
		});
		GetOwner()->GetWorldTimerManager().SetTimer(TickTimerHandle, TimerDelegate, 1.0f, true);
	}
	// Note: The server listens for incoming connections and uses a separate socket for sending responses to Python.
	return true;
}

// Send a response back to Python on a specified port.
bool UPythonCommunicationComponentTG::SendMessage(FMessageTG& Message) 
{
	TRACE("%s (Reg Id: %d) is sending message", *GetOwner()->GetName(), RegisterId);
	bool result = false;

	if (!ClientSocket)
	{
		TRACEERROR("ClientSocket is invalid!");
		return result;
	}

	Payload messageBuffer;
	if (!Message.Serialize(messageBuffer))
	{
		TRACEERROR("Failed to serialize data!");
		return result;
	}
	int32 payloadLen = messageBuffer.Num();
	int32 bytesSent = 0;

	TRACE("%d of data to be sent (hex): ", payloadLen)
	TRACEBYTES(messageBuffer.GetData(), payloadLen)
	result = ClientSocket->Send(messageBuffer.GetData(), payloadLen, bytesSent);
	if (bytesSent == payloadLen)
		TRACE("Data has been sent successfully")
	else
	{
		TRACEERROR("BytesSent is not equal to Payload length!")
		result = false;
	}

	return result;
}

// Handles the incoming data from connected clients (Python script)
bool UPythonCommunicationComponentTG::HandleData(FMessageTG&& Message)
{
	TRACE("")
	if (!bShouldHandleData)
	{
		TRACEWARN("UE System is currently busy");
		return false;
	}

	if (!ClientSocket)
	{
		TRACEERROR("Listener server stopped!");
		return false;
	}

	if (!MessageDispacher)
	{
		TRACEERROR("MessageDispacher is missing!");
		return false;
	}

	uint8* valuePtr = Message.DataSignal.GetValue<uint8>();
	if (MsgType::Unregister == Message.Type && valuePtr && 1 == *valuePtr)
	{
		bIsRegistered = false;
		TRACE("Client %s (regId: %d) unregistered successfully!", 
			*GetOwner()->GetName(), RegisterId)
	}

	TRACE("Received new message")
	MessageDispacher->AddReceivedMessage(MoveTemp(Message));
	return true;
}

bool UPythonCommunicationComponentTG::HandleConnection(FMessageTG&& Message)
{
	TRACE("%s is connecting to the server with id: %d", 
		*GetOwner()->GetName(), RegisterId)
	bool result = false;

	if (!bShouldHandleData)
	{
		TRACEWARN("UE System is currently busy");
		return false;
	}

	uint8* valuePtr = Message.DataSignal.GetValue<uint8>();
	if (MsgType::Register == Message.Type && valuePtr && 1 == *valuePtr)
	{
		bIsRegistered = true;
		TRACE("Client %s (regId: %d) registered successfully!", 
			*GetOwner()->GetName(), RegisterId)
		result = true;
	}
	else
	{
		TRACE("It's not a register message!")
		result = false;
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

bool UPythonCommunicationComponentTG::ReceiveMessage()
{
	bool result = false;
	if (!ClientSocket)
	{
		TRACEERROR("Listener server not connected!");
		return false;
	}

	if (ClientSocket->GetConnectionState() == SCS_Connected)
	{
		uint32 pendingSize = 0;
		while (ClientSocket->HasPendingData(pendingSize))
		{
			TRACE("%d bytes of payload waits to be read", pendingSize)

			if (MIN_MSG_SIZE > pendingSize)
			{
				TRACE("Pending message size is too small!")
				return false;
			}
			FMessageTG msg = FMessageTG();
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

			TRACE("Data received (hex): ")
			TRACEBYTES(payload.GetData(), payload.Num())

			size_t bytes = msg.Deserialize(payload);
			TRACE("Deserialized %d bytes of received data", bytes)
			if (msg.DataSignal.IsEmpty())
			{
				TRACE("Received Message Data is empty!")
				result = false;
				continue;
			}
			if (bIsRegistered)
			{
				if (!HandleData(std::move(msg)))
				{
					TRACE("Failed to handle data");
					return false;
				}
			}
			else if (!HandleConnection(std::move(msg)))
			{
				TRACE("Failed to connect");
				return false;
			}
			result = true;
		}
	}
	else
	{
		TRACEERROR("Client socket is not connected!");
		return false;
	}
	return result;
}

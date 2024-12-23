// Fill out your copyright notice in the Description page of Project Settings.

#include "PythonCommunicationComponentTG.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Networking.h"
#include "Sockets.h"

#include "TheGame/TheGame.h"


uint8_t UPythonCommunicationComponentTG::CurrRegisterId{0};

// Sets default values for this component's properties
UPythonCommunicationComponentTG::UPythonCommunicationComponentTG()
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
	// Py_Initialize();

	// File opened
	// FILE* fp = nullptr;
	// fopen_s(fp, "Source/Python/ue_communication_server.py", "w");
	// int result = PyRun_AnyFile(fp, "Source/Python/script.py");
	// if(GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Yellow,
	// 	FString::Printf(TEXT("result = %d"), result));
	// } else
	// {
	// UE_LOG(LogTemp, Error, TEXT("PyRun_AnyFile result "));
	// }
	// StartServer();
	ConnectToServer();
}

void UPythonCommunicationComponentTG::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	TRACE("");
	EndServer();
	// EndConnection();
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

// Both functions have to exist so they can be called from Unreal Engine
// Called when Avatar starts signing. Pauses the processing of incoming data.
void UPythonCommunicationComponentTG::PauseHandlingData()
{
	TRACE("");
	bShouldHandleData = false;
}

// Called when Avatar ends signing. Resumes the processing of incoming data.
void UPythonCommunicationComponentTG::ResumeHandlingData()
{
	TRACE("");
	bShouldHandleData = true;
}

// Start the server to listen for incoming data, and define a socket for sending a response
void UPythonCommunicationComponentTG::StartServer() 
{
	TRACE("");
	// Sets up a TCP server on localhost:PORT.
	FIPv4Endpoint ListenerEndpoint(FIPv4Address(127, 0, 0, 1), ServerPort);

	// Listening for incoming connections
	ReceivingSocket = FTcpSocketBuilder(TEXT("Python Listener"))
		.AsReusable() // Can be used after being deleted (after the server is closed), without having to wait some time
		.BoundToEndpoint(ListenerEndpoint) // Binds it to the Endpoint
		.Listening(8); // Max number of pending connections

	// Define a socket that will send a response back to Python when necessary 
	SendingSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)
		->CreateSocket(NAME_Stream, TEXT("Send response to Python"), false);

	if (ReceivingSocket) // If ReceivingSocket was created successfully
	{
		// Initializes a timer to periodically handle incoming data from connected clients (Python script).
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]() 
			{ 
				HandleData(); 
			});
		
		GetOwner()->GetWorldTimerManager().SetTimer(TickTimerHandle, TimerDelegate, 1.0f, true);

		TRACE("Listener server started");
	}

	// Note: The server listens for incoming connections and uses a separate socket for sending responses to Python.
}

// Send a response back to Python on a specified port.
void UPythonCommunicationComponentTG::SendMessage(DroneTrainerCommTG::Message& Message) 
{
	TRACE("");
	if (SendingSocket) // If SendingSocket was created successfully
	{
		// Connects to the Python server using the SendingSocket.
		FIPv4Endpoint SendEndpoint(FIPv4Address(127, 0, 0, 1), ServerPort);
		SendingSocket->Connect(*SendEndpoint.ToInternetAddr());
		
		// Sending the response data.
		TTuple<DroneTrainerCommTG::PayloadLen,
				const DroneTrainerCommTG::Byte*> MessageBuffer = Message.Serialize();
		const uint8* Data = MessageBuffer.Value;
		int32 PayloadLen = MessageBuffer.Key;
		int32 BytesSent = 0;
		TRACE("Data to be sent:")
		FString DataHex;
		for (int32 i=0;i<PayloadLen;i++)
			DataHex.Append(FString::Printf(TEXT("0x%02x "), Data[i]));

		TRACE("Data to be sent: (hex): %s", *DataHex)

		SendingSocket->Send(Data, PayloadLen, BytesSent);

		if (BytesSent == PayloadLen)
		{
			TRACE("Data has been sent successfully");
		}
	}
}

// Handles the incoming data from connected clients (Python script)
void UPythonCommunicationComponentTG::HandleData()
{
	TRACE("");
	// Note: Data handling may be paused by Unreal Engine using bShouldHandleData flag.
	if (!bShouldHandleData) // Data handling possibly paused by Unreal Engine
	{
		TRACEWARN("UE System is currently busy");
		return;
	}

	if (!ReceivingSocket) // If socket is not valid for any reason stop 
	{
		TRACEERROR("Listener server stopped!");
		return;
	}

	if (ReceivingSocket && ReceivingSocket->GetConnectionState() == SCS_Connected)
	{
		// Receives data from the active socket.
		char Buffer[1024] = { 0 };
		int32 BytesRead = 0;
		ReceivingSocket->Recv((uint8*)Buffer, sizeof(Buffer), BytesRead);
		
		// Converts the data to FString and triggers an event (inside of Unreal Engine)
		ReceivedData = FString(FUTF8ToTCHAR(Buffer));
		FString ReceivedDataHex;
		
		for (int32 i=0;i<ReceivedData.Len();i++)
			ReceivedDataHex.Append(FString::Printf(TEXT("0x%02x "), Buffer[i]));
		TRACE("Data received (hex): %s", *ReceivedDataHex)
		TRACE("Data received: %s", *ReceivedData)
		OnReceivedDataChanged();
	}
}

void UPythonCommunicationComponentTG::HandleConnection()
{
	TRACE("");
	// Note: Data handling may be paused by Unreal Engine using bShouldHandleData flag.
	if (!bShouldHandleData) // Data handling possibly paused by Unreal Engine
	{
		TRACEWARN("UE System is currently busy");
		return;
	}

	if (!ReceivingSocket) // If socket is not valid for any reason stop 
	{
		TRACEERROR("Listener server stopped!");
		return;
	}

	DroneTrainerCommTG::Signal EmptySignal;
	CurrentSignalsBuffer = TArray{std::move(EmptySignal)};
	DroneTrainerCommTG::Message RegisterMsg =
		DroneTrainerCommTG::Message(CurrRegisterId, DroneTrainerCommTG::MsgType::Register, CurrentSignalsBuffer);
	SendMessage(RegisterMsg);

	CurrentSignalsBuffer.Empty();
	
	if (ReceivingSocket && ReceivingSocket->GetConnectionState() == SCS_Connected)
	{
		// Receives data from the active socket.
		char Buffer[MAX_DATA_SIZE] = { 0 };
		int32 BytesRead = 0;
		ReceivingSocket->Recv((uint8*)Buffer, sizeof(Buffer), BytesRead);
		
		// Converts the data to FString and triggers an event (inside of Unreal Engine)
		ReceivedData = FString(FUTF8ToTCHAR(Buffer));
		FString ReceivedDataHex;
		
		for (int32 i=0;i<ReceivedData.Len();i++)
			ReceivedDataHex.Append(FString::Printf(TEXT("0x%02x "), Buffer[i]));

		TRACE("Data received (hex): %s", *ReceivedDataHex)
		TRACE("Data received: %s", *ReceivedData)
		TArray<DroneTrainerCommTG::Signal> data;
		DroneTrainerCommTG::Message msg =
			DroneTrainerCommTG::Message(0, DroneTrainerCommTG::MsgType::None, data);
		msg.Deserialize((uint8*)Buffer);
		if (1 == std::get<uint8>(msg.Data[0].Value))
			isRegistered = true;
	}
}

// Closes the server socket and cleans up associated resources.
// Also closes the socket used for sending responses to Python.
void UPythonCommunicationComponentTG::EndServer()
{
	TRACE("");
	if (ReceivingSocket)
	{
		ReceivingSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ReceivingSocket);
		ReceivingSocket = nullptr;
		UE_LOG(LogTemp, Log, TEXT("Listener server ended."));
	}

	if (SendingSocket)
	{
		SendingSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(SendingSocket);
		SendingSocket = nullptr;
		UE_LOG(LogTemp, Log, TEXT("Send server ended"));
	}
}

void UPythonCommunicationComponentTG::ConnectToServer()
{
	TRACE("");
	// Sets up a TCP client connection with server on localhost:PORT.
	FIPv4Endpoint ListenerEndpoint(FIPv4Address(127, 0, 0, 1), ServerPort);

	// Listening for incoming connections
	ReceivingSocket = FTcpSocketBuilder(TEXT("Python Listener"))
		.AsReusable(); // Can be used after being deleted (after the server is closed), without having to wait some time
		// .BoundToEndpoint(ListenerEndpoint) // Binds it to the Endpoint
		// .Listening(8); // Max number of pending connections

	if (!ReceivingSocket->Connect(*ListenerEndpoint.ToInternetAddrIPV4()))
	{
		TRACEERROR("The connection to server has failed!");
	}
	
	// Define a socket that will send a response back to Python when necessary 
	SendingSocket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)
		->CreateSocket(NAME_Stream, TEXT("Send response to Python"), false);
	
	
	if (ReceivingSocket) // If ReceivingSocket was created successfully
	{
		// Initializes a timer to periodically handle incoming data from connected clients (Python script).
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([this]() 
			{
				if (isRegistered)
					HandleData();
				else
					HandleConnection();
			});
		
		GetOwner()->GetWorldTimerManager().SetTimer(TickTimerHandle, TimerDelegate, 1.0f, true);

		TRACE("Listener server started");
	}

	// Note: The server listens for incoming connections and uses a separate socket for sending responses to Python.
}

void UPythonCommunicationComponentTG::EndConnection()
{
	TRACE("");

}

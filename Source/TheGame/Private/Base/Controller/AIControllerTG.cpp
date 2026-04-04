// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/Controller/AIControllerTG.h"

#include "TheGame/TheGame.h"
#include "Base/CharacterTG.h"
#include "PythonCommunication/PythonCommunicationComponentTG.h"
#include "PythonCommunication/MessageDispatcherTG.h"


AAIControllerTG::AAIControllerTG() :
    Super(),
    RegisterId{ 0 },
    bDelegatesRegistered{ false }
{

}

AAIControllerTG::~AAIControllerTG()
{
}

void AAIControllerTG::BeginPlay()
{
}

void AAIControllerTG::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);
    // Store a reference to the Player's Pawn
    TRACE("");
    ControlledCharacter = Cast<ACharacterTG>(aPawn);
    checkf(ControlledCharacter,
        TEXT("APlayerControllerTG derived classes should only posses ACharacterTG derived pawns"));
    
    CommunicationComponent = 
        aPawn->FindComponentByClass<UPythonCommunicationComponentTG>();
    if (!CommunicationComponent)
    {
        TRACEERROR("CommunicationComponent is null!");
        return;
    }
    MessageDispacher = CommunicationComponent->MessageDispacher;
    RegisterId = CommunicationComponent->RegisterId;
    if (!MessageDispacher)
    {
        TRACEERROR("MessageDispacher is null!");
        return;
    }

    bDelegatesRegistered = RegisterDelegates();

    TimerDelegate.BindLambda([this]()
    {
        if (!bDelegatesRegistered)
            bDelegatesRegistered = RegisterDelegates();
        SendSnapshot();
    });
    GetWorld()->GetTimerManager().SetTimer(
        TickTimerHandle, TimerDelegate, 1.0f, true);
}

void AAIControllerTG::OnUnPossess()
{
    Super::OnUnPossess();

    if (MessageDispacher)
    {
        MessageDispacher->OnDroneMovementReceived.RemoveAll(this);
        MessageDispacher->OnDroneLookReceived.RemoveAll(this);
    }
}

bool AAIControllerTG::RegisterDelegates()
{
    bool result = true;

    if (!MoveDelegateHandle.IsValid())
    {
        MoveDelegateHandle = MessageDispacher->OnDroneMovementReceived.AddUObject(
            this,
            &FControllerTG::HandleMove
        );
    }
    if (!LookDelegateHandle.IsValid())
    {
        LookDelegateHandle = MessageDispacher->OnDroneLookReceived.AddUObject(
            this,
            &AAIControllerTG::HandleLook
        );
    }

    if (!MoveDelegateHandle.IsValid())
    {
        TRACEWARN("MoveDelegateHandle is invalid!");
        MessageDispacher->OnDroneMovementReceived.Remove(MoveDelegateHandle);
        result = false;
    }
    if (!LookDelegateHandle.IsValid())
    {
        TRACEWARN("LookDelegateHandle is invalid!");
        MessageDispacher->OnDroneMovementReceived.Remove(LookDelegateHandle);
        result = false;
    }
    return result;
}

void AAIControllerTG::SendSnapshot()
{
    TRACE("");
    if (!CommunicationComponent->IsConnected())
    {
        TRACEERROR("CommunicationComponent is not connected!");
        return;
    }
    if (!CommunicationComponent->bIsRegistered)
    {
        TRACEERROR("CommunicationComponent is not registered to server!");
        return;
    }
    ACharacterTG* character = Cast<ACharacterTG>(GetPawn());
    if (!character)
    {
        TRACEERROR("Character is null!");
        return;
    }
    if (!MessageDispacher)
    {
        TRACEERROR("MessageDispacher is null!");
        return;
    }
    FVector location = character->GetActorLocation();
    FRotator rotation = character->GetActorRotation();

    MessageDispacher->ComposeSnapshotMessage(
        RegisterId,
        location, 
        rotation, 
        character->bDidDamage,
        character->bDidKill,
        character->bWasHit,
        character->bWasKilled);

    character->bDidDamage = false;
    character->bDidKill = false;
    character->bWasHit = false;
    character->bWasKilled = false;
}

void AAIControllerTG::HandleLook(const FInputActionValue& InputActionValue)
{
    const FVector2D lookAxisVector = InputActionValue.Get<FVector2D>();
    TRACE("Look X: %f / Y: %f", lookAxisVector.X, lookAxisVector.Y);
    FRotator controlRot = this->GetControlRotation();

    controlRot.Yaw += lookAxisVector.X * Sensitivity;
    controlRot.Pitch += lookAxisVector.Y * Sensitivity;

    // Clamp pitch (important)
    controlRot.Pitch = FMath::Clamp(controlRot.Pitch, -80.f, 80.f);

    this->SetControlRotation(controlRot);
}

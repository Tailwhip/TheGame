// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/Controller/AIControllerTG.h"

#include "TheGame/TheGame.h"
#include "Base/CharacterTG.h"
#include "PythonCommunication/PythonCommunicationComponentTG.h"
#include "PythonCommunication/MessageDispatcherTG.h"


AAIControllerTG::AAIControllerTG() :
    Super()
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
    TRACE("")
    ControlledCharacter = Cast<ACharacterTG>(aPawn);
    checkf(ControlledCharacter,
        TEXT("APlayerControllerTG derived classes should only posses ACharacterTG derived pawns"));
    
    MessageDispacher =
        aPawn->FindComponentByClass<UPythonCommunicationComponentTG>()->MessageDispacher;
    if (!MessageDispacher) TRACEERROR("MessageDispacher is null!")

    MessageDispacher->OnDroneMovementReceived.AddUObject(
        this,
        &FControllerTG::HandleMove
    );
    MessageDispacher->OnDroneLookReceived.AddUObject(
        this,
        &AAIControllerTG::HandleLook
    );
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

void AAIControllerTG::HandleLook(const FInputActionValue& InputActionValue)
{
    TRACE("")
    // Input is a Vector2D
    const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

    FRotator ControlRot = this->GetControlRotation();

    ControlRot.Yaw += LookAxisVector.X * Sensitivity;
    ControlRot.Pitch += LookAxisVector.Y * Sensitivity;

    // Clamp pitch (important)
    ControlRot.Pitch = FMath::Clamp(ControlRot.Pitch, -80.f, 80.f);

    this->SetControlRotation(ControlRot);
}

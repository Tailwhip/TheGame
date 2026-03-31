// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/Controller/ControllerTG.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/GameEngine.h"
#include "GameFramework/PawnMovementComponent.h"

#include "TheGame/TheGame.h"
#include "Base/CharacterTG.h"


FControllerTG::FControllerTG()
{
}

FControllerTG::~FControllerTG()
{
}


void FControllerTG::HandleMove(const FInputActionValue& InputActionValue)
{
    TRACE("")
    // Input is a Vector2D
    // const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
    const FVector MovementVector = InputActionValue.Get<FVector>();
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow,
            FString::Printf(TEXT(" %s"), *MovementVector.ToString()));
    }
    // Add movement to the Player's Character Pawn
    if (ControlledCharacter)
    {
        ControlledCharacter->AddMovementInput(
            ControlledCharacter->GetActorForwardVector(), MovementVector.Y);
        ControlledCharacter->AddMovementInput(
            ControlledCharacter->GetActorRightVector(), MovementVector.X);
    }
}

void FControllerTG::HandleJump()
{
    // Input is 'Digital' (value not used here)

    // Make the Player's Character Pawn jump, disabling crouch if it was active
    if (ControlledCharacter)
    {
        ControlledCharacter->UnCrouch();
        ControlledCharacter->Jump();
    }
}

void FControllerTG::HandleCrouch()
{
    // Input is 'Digital' (value not used here)

    // Make the Player's Character Pawn crouch
    if (ControlledCharacter && ControlledCharacter->bIsCrouched)
    {
        ControlledCharacter->UnCrouch();
    }
    else
    {
        ControlledCharacter->Crouch();
    }
}

void FControllerTG::HandleAscend(const FInputActionValue& InputActionValue)
{
    const FVector MovementVector = InputActionValue.Get<FVector>();
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow,
            FString::Printf(TEXT(" %s"), *MovementVector.ToString()));
    }

    // Add movement to the Player's Character Pawn
    if (ControlledCharacter)
    {
        // TUniquePtr<UNavMovementComponent> charMoveComp(ControlledCharacter->GetMovementComponent());
        // charMoveComp->MovementState.bCanFly = true;
        ControlledCharacter->AddMovementInput(ControlledCharacter->GetActorUpVector(), 1);
    }
}

void FControllerTG::HandleDescend(const FInputActionValue& InputActionValue)
{
    const FVector MovementVector = InputActionValue.Get<FVector>();
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow,
            FString::Printf(TEXT(" %s"), *MovementVector.ToString()));
    }

    // Add movement to the Player's Character Pawn
    if (ControlledCharacter)
    {
        // TUniquePtr<UNavMovementComponent> charMoveComp(ControlledCharacter->GetMovementComponent());
        // charMoveComp->MovementState.bCanFly = true;
        ControlledCharacter->AddMovementInput(ControlledCharacter->GetActorUpVector(), -1);
    }
}

void FControllerTG::HandleShooting()
{
    if (ControlledCharacter)
    {
        TRACE("Shooting projectile...")
        ControlledCharacter->ShootProjectile();
    }
    else
    {
        TRACEERROR("ControlledCharacter is missing!")
    }
}

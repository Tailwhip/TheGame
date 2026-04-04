// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/Controller/PlayerControllerTG.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/GameEngine.h"
#include "GameFramework/PawnMovementComponent.h"

#include "TheGame/TheGame.h"
#include "Base/CharacterTG.h"


void APlayerControllerTG::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    // Store a reference to the Player's Pawn
    ControlledCharacter = Cast<ACharacterTG>(aPawn);
    checkf(ControlledCharacter,
        TEXT("APlayerControllerTG derived classes should only posses ACharacterTG derived pawns"));
    
    // Get a reference to the EnhancedInputComponent.
    EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
    checkf(EnhancedInputComponent, 
        TEXT("Unable to get reference to the EnhancedInputComponent."));

    // Get the local player subsystem
    UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    checkf(InputSubsystem, 
        TEXT("Unable to get references to the EnhancedInputLocalSubsystem."));

    // Wipe existing mappings, and add our mapping.
    checkf(InputMappingContext, TEXT("InputMappingContext was not specified."));
    InputSubsystem->ClearAllMappings();
    InputSubsystem->AddMappingContext(InputMappingContext, 0);

    if (EnhancedInputComponent)
    {
        // Bind the input actions.
        // Only attempt to bind if a valid value was provided.
        if (ActionMove)
            EnhancedInputComponent->BindAction(
                ActionMove, 
                ETriggerEvent::Triggered, this, 
                &FControllerTG::HandleMove);

        if (ActionLook)
            EnhancedInputComponent->BindAction(
                ActionLook, 
                ETriggerEvent::Triggered, this, 
                &APlayerControllerTG::HandleLook);

        if (ActionJump)
            EnhancedInputComponent->BindAction(
                ActionJump, 
                ETriggerEvent::Triggered, this, 
                &FControllerTG::HandleJump);
    
        if (ActionCrouch)
            EnhancedInputComponent->BindAction(
                ActionCrouch, 
                ETriggerEvent::Triggered, this, 
                &FControllerTG::HandleCrouch);

        if (ActionAscend)
            EnhancedInputComponent->BindAction(
                ActionAscend, 
                ETriggerEvent::Triggered, this, 
                &FControllerTG::HandleAscend);
    
        if (ActionDescend)
            EnhancedInputComponent->BindAction(
                ActionDescend, 
                ETriggerEvent::Triggered, this, 
                &FControllerTG::HandleDescend);
    
        if (ActionLClick)
            EnhancedInputComponent->BindAction(
                ActionLClick,
                ETriggerEvent::Triggered,
                this,
                &FControllerTG::HandleShooting);
    }
}

void APlayerControllerTG::HandleLook(const FInputActionValue& InputActionValue)
{
    const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();
    //TRACE("Look X: %f / Y: %f", LookAxisVector.X, LookAxisVector.Y);

    // Add yaw and pitch input to controller
    AddYawInput(LookAxisVector.X * Sensitivity);
    AddPitchInput(LookAxisVector.Y * Sensitivity);
}

void APlayerControllerTG::OnUnPossess()
{
    // Unbind things here...
    EnhancedInputComponent->ClearActionBindings();

    // Call the parent class method
    Super::OnUnPossess();
}

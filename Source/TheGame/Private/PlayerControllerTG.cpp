// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerTG.h"
#include "CharacterTG.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "TheGame/TheGame.h"
#include "Engine/GameEngine.h"
#include "GameFramework/PawnMovementComponent.h"


void APlayerControllerTGBase::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);
    
    // Store a reference to the Player's Pawn
    PlayerCharacter = Cast<ACharacterTG>(aPawn);
    checkf(PlayerCharacter, 
        TEXT("APlayerControllerTGBase derived classes should only posses ACharacterTGBase derived pawns"));
    
    // Get a reference to the EnhancedInputComponent.
    EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
    checkf(EnhancedInputComponent, 
        TEXT("Unable to get reference to the EnhancedInputComponent."));

    // Get the local player subsystem
    UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
    checkf(InputSubsystem, TEXT("Unable to get references to the EnhancedInputLocalSubsystem."));

    // Wipe existing mappings, and add our mapping.
    checkf(InputMappingContext, TEXT("InputMappingContext was not specified."));
    InputSubsystem->ClearAllMappings();
    InputSubsystem->AddMappingContext(InputMappingContext, 0);
    
    // Bind the input actions.
    // Only attempt to bind if a valid value was provided.
    if (ActionMove)
        EnhancedInputComponent->BindAction(
            ActionMove, 
            ETriggerEvent::Triggered, this, 
            &APlayerControllerTGBase::HandleMove);

    if (ActionLook)
        EnhancedInputComponent->BindAction(
            ActionLook, 
            ETriggerEvent::Triggered, this, 
            &APlayerControllerTGBase::HandleLook);

    if (ActionJump)
        EnhancedInputComponent->BindAction(
            ActionJump, 
            ETriggerEvent::Triggered, this, 
            &APlayerControllerTGBase::HandleJump);
    
    if (ActionCrouch)
        EnhancedInputComponent->BindAction(
            ActionCrouch, 
            ETriggerEvent::Triggered, this, 
            &APlayerControllerTGBase::HandleCrouch);

    if (ActionAscend)
        EnhancedInputComponent->BindAction(
            ActionAscend, 
            ETriggerEvent::Triggered, this, 
            &APlayerControllerTGBase::HandleAscend);
    
    if (ActionDescend)
        EnhancedInputComponent->BindAction(
            ActionDescend, 
            ETriggerEvent::Triggered, this, 
            &APlayerControllerTGBase::HandleDescend);
}

void APlayerControllerTGBase::OnUnPossess()
{
    // Unbind things here...
    EnhancedInputComponent->ClearActionBindings();

    // Call the parent class method
    Super::OnUnPossess();
}

void APlayerControllerTGBase::ProcessPlayerInput(const float DeltaTime, const bool bGamePaused)
{
    Super::ProcessPlayerInput(DeltaTime, bGamePaused);

    if (bGamePaused) return;

    if (IsInputKeyDown(EKeys::RightMouseButton) || IsInputKeyDown(EKeys::LeftMouseButton))
    {
        int a = 0;
    }
    if (WasInputKeyJustPressed(EKeys::LeftMouseButton))
    {
        TRACE("Shooting projectile...")
        PlayerCharacter->ShootProjectile();
    }
    if (WasInputKeyJustPressed(EKeys::RightMouseButton))
    {
        int a = 0;
    }
    if (WasInputKeyJustPressed(EKeys::MiddleMouseButton))
    {
        int a = 0;
    }
    if (WasInputKeyJustPressed(EKeys::ThumbMouseButton))
    {
        int a = 0;
    }
    if (WasInputKeyJustPressed(EKeys::ThumbMouseButton2))
    {
        int a = 0;
    }
}

void APlayerControllerTGBase::HandleMove(const FInputActionValue& InputActionValue)
{
    // Input is a Vector2D
    // const FVector2D MovementVector = InputActionValue.Get<FVector2D>();
    const FVector MovementVector = InputActionValue.Get<FVector>();
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow,
        FString::Printf(TEXT(" %s"), *MovementVector.ToString()));
    }
    // Add movement to the Player's Character Pawn
    if (PlayerCharacter)
    {
        PlayerCharacter->AddMovementInput(
            PlayerCharacter->GetActorForwardVector(), MovementVector.Y);
        PlayerCharacter->AddMovementInput(
            PlayerCharacter->GetActorRightVector(), MovementVector.X);
    }
}

void APlayerControllerTGBase::HandleLook(const FInputActionValue& InputActionValue)
{
    // Input is a Vector2D
    const FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

    // Add yaw and pitch input to controller
    AddYawInput(LookAxisVector.X);
    AddPitchInput(LookAxisVector.Y);
}

void APlayerControllerTGBase::HandleJump()
{
    // Input is 'Digital' (value not used here)

    // Make the Player's Character Pawn jump, disabling crouch if it was active
    if (PlayerCharacter)
    {
        PlayerCharacter->UnCrouch();
        PlayerCharacter->Jump();
    }
}

void APlayerControllerTGBase::HandleCrouch()
{
    // Input is 'Digital' (value not used here)

    // Make the Player's Character Pawn crouch
    if (PlayerCharacter && PlayerCharacter->bIsCrouched)
    {
        PlayerCharacter->UnCrouch();
    }
    else
    {
        PlayerCharacter->Crouch();
    }
}

void APlayerControllerTGBase::HandleAscend(const FInputActionValue& InputActionValue)
{
    const FVector MovementVector = InputActionValue.Get<FVector>();
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow,
        FString::Printf(TEXT(" %s"), *MovementVector.ToString()));
    }
    
    // Add movement to the Player's Character Pawn
    if (PlayerCharacter)
    {
        // TUniquePtr<UNavMovementComponent> charMoveComp(PlayerCharacter->GetMovementComponent());
        // charMoveComp->MovementState.bCanFly = true;
        PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorUpVector(), 1);
    }
}

void APlayerControllerTGBase::HandleDescend(const FInputActionValue& InputActionValue)
{
    const FVector MovementVector = InputActionValue.Get<FVector>();
    if(GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow,
        FString::Printf(TEXT(" %s"), *MovementVector.ToString()));
    }
    
    // Add movement to the Player's Character Pawn
    if (PlayerCharacter)
    {
        // TUniquePtr<UNavMovementComponent> charMoveComp(PlayerCharacter->GetMovementComponent());
        // charMoveComp->MovementState.bCanFly = true;
        PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorUpVector(), -1);
    }
}

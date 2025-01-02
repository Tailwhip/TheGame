// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PlayerControllerTG.generated.h"


// Forward Declarations

class UEnhancedInputComponent;
class ACharacterTG;
class UInputAction;
class UInputMappingContext;

/**
 * 
 */
UCLASS(Abstract)
class THEGAME_API APlayerControllerTGBase : public APlayerController
{
	GENERATED_BODY()

public:
	// The Input Action to map to movement.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionMove = nullptr;

	// The Input Action to map to looking around.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionLook = nullptr;

	// The Input Action to map to jumping.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionJump = nullptr;

	// The Input Action to map to crouching.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionCrouch = nullptr;

	// The Input Action to map to ascending.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionAscend = nullptr;

	// The Input Action to map to descending.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionDescend = nullptr;
	
	// The Input Mapping Context to use
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player Input|Character Movement");
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;

protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;
	virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused) override;
	
	void HandleMove(const FInputActionValue& InputActionValue);
	void HandleLook(const FInputActionValue& InputActionValue);
	void HandleJump();
	void HandleCrouch();
	void HandleAscend(const FInputActionValue& InputActionValue);
	void HandleDescend(const FInputActionValue& InputActionValue);

	// Used to store a reference to the InputComponent cast to an EnhancedInputComponent.
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = nullptr;

	// Used to store a reference to the pawn we are controlling.
	UPROPERTY()
	TObjectPtr<ACharacterTG> PlayerCharacter = nullptr;
};

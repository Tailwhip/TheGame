// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectileTG.h"
#include "Camera/CameraComponent.h"
// #include "MLAdapterLocDataCollectSessionTG.h"
// #include "Agents/MLAdapterAgent.h"

#include "CharacterTG.generated.h"


UCLASS()
class THEGAME_API ACharacterTG : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterTG();

	virtual void BeginPlay() override;

	virtual  void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ShootProjectile() const;

	// UPROPERTY(EditAnywhere, meta=(TitleProperty="Projectile"), Category = "Projectile")
	// AActor Projectile;
	//
	UPROPERTY(EditAnywhere, Category = Projectile)
	TSubclassOf<class AProjectileTG> ProjectileClass;
	
protected:
	UPROPERTY()
	TObjectPtr<UCameraComponent> Camera{nullptr};

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UDroneHudTG> DroneHudClass{nullptr};

	UPROPERTY()
	class UDroneHudTG* DroneHud{nullptr};
};

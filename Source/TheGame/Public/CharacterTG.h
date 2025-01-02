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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	TObjectPtr<UCameraComponent> Camera = nullptr;
};

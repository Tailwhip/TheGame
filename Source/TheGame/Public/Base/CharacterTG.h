// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectileTG.h"

#include "CharacterTG.generated.h"


UCLASS(Abstract)
class THEGAME_API ACharacterTG : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterTG();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
		
	void ShootProjectile() const;

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly)
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly)
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class APlayerControllerTG* PlayerController;
	
protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UDroneHudTG> DroneHudClass{nullptr};
	
	UPROPERTY()
	class UDroneHudTG* DroneHud{nullptr};
	
	UPROPERTY(EditAnywhere, Category = Projectile)
	TSubclassOf<class AProjectileTG> ProjectileClass{nullptr};

	UPROPERTY(EditAnywhere)
	float HP{100.0};

	ACharacterTG* ThisCharacter{nullptr};
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectileTG.h"
#include "CharacterTG.generated.h"


class UActorsSpawnerComponentTG;

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
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;
	
	void ShootProjectile() const;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileTG")
	class USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileTG")
	class UActorsSpawnerComponentTG* ProjectileSpawnerComponent;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterTG")
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterTG")
	class USpringArmComponent* CameraBoom;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, 
		Category = "InputTG", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, 
		Category = "InputTG", meta = (AllowPrivateAccess = "true"))
	class APlayerControllerTG* PlayerController;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "CharacterTG")
	TSubclassOf<class UDroneHudTG> DroneHudClass{nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "CharacterTG")
	class UDroneHudTG* DroneHud{nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileTG")
	float TraceDistance = 100000.f;

	UPROPERTY(EditDefaultsOnly, Category = "StatsTG")
	float HealthPoints{100.0};

	UPROPERTY(EditDefaultsOnly, Category = "StatsTG")
	float AttackPower{ 10.0 };

	UPROPERTY(EditDefaultsOnly, Category = "FeaturesTG")
	bool IsDestroyable{ false };

	ACharacterTG* ThisCharacter{nullptr};

	UFUNCTION()
	void BeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;
};

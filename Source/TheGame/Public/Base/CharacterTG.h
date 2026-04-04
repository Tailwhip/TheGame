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
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;
	
	void ShootProjectile() const;
	void OnHealthChanged(float NewHealth);
	//void OnAmmoChanged(int32 Ammo);
	void OnDeath();
	void OnHit(AActor* OtherActor); // The opponent
	void OnKill(); // The opponent

	UPROPERTY(EditDefaultsOnly, Category = "CharacterTG")
	class USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterTG")
	class UActorsSpawnerComponentTG* ProjectileSpawnerComponent;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterTG")
	class UPythonCommunicationComponentTG* PythonCommunicationComponent;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterTG")
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterTG")
	class USpringArmComponent* CameraBoom;

	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterTG")
	//class UInputMappingContext* DefaultMappingContext;

	//UPROPERTY()
	//class APlayerControllerTG* PlayerController;

	//UPROPERTY()
	//class AAIControllerTG* AIController;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterTG")
	TSubclassOf<class UCharacterHUDWidgetTG> HUDClass{nullptr};

	UPROPERTY()
	class UCharacterHUDWidgetTG* HUDWidget{nullptr};

	UPROPERTY(EditDefaultsOnly, Category = "CharacterTG")
	float TraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterTG")
	float HealthPoints;
	
	UPROPERTY(EditDefaultsOnly, Category = "CharacterTG")
	float AttackPower;

	UPROPERTY(EditInstanceOnly, Category = "CharacterTG")
	bool bIsDestroyable;

	UPROPERTY(EditInstanceOnly, Category = "CharacterTG")
	bool bIsAI;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterTG")
	bool bDidDamage;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterTG")
	bool bDidKill;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterTG")
	bool bWasHit;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterTG")
	bool bWasKilled;

protected:
	ACharacterTG* ThisCharacter{nullptr};
	UFUNCTION()
	void BeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser) override;
private:
	float MaxHealth;
};

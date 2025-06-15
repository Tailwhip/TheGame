// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Base/ActorTG.h"
#include "ProjectileTG.generated.h"

UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class THEGAME_API AProjectileTG : public AActorTG
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileTG();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* ProjectileFX;
	
	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* ImpactParticles;
	
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class USphereComponent* CollisionSphere;

	UPROPERTY(EditDefaultsOnly)
	float BaseDamage{10.f};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageType> DamageType;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void BeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComponent,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	void OnHit(AActor* OtherActor);
};

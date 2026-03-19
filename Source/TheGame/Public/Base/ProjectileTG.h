// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorTG.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectileTG.generated.h"

class UMeshComponent;
class UShapeComponent;
class UArrowComponent;


UCLASS(BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class THEGAME_API AProjectileTG : public AActorTG
{
	GENERATED_BODY()
	
public:	
	AProjectileTG();
	// Called when the game starts or when spawned
	void OnConstruction(const FTransform& Transform) override;
	void BeginPlay() override;
	void Tick(float DeltaTime) override; // Called every frame

	void Activate() override;
	void ResetMovement(FTransform const& Transform) override;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileTG")
	class UShapeComponent* CollisionShape; // Root

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileTG")
	TSubclassOf<UShapeComponent> CollisionShapeType;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileTG")
	class UMeshComponent* ProjectileMesh;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileTG")
	class UArrowComponent* ProjectileDirectionArrow;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileTG")
	class UNiagaraComponent* ProjectileFX;
	
	UPROPERTY(EditDefaultsOnly, Category = "ProjectileTG")
	class UNiagaraSystem* ImpactParticles;
	
	UPROPERTY(EditDefaultsOnly, Category = "ProjectileTG")
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileTG")
	TSubclassOf<UProjectileMovementComponent> ProjectileMovementType;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileTG")
	float BaseDamage{10.f};

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileTG")
	TSubclassOf<UDamageType> DamageType;

private:
	// Sets default values for this actor's properties
	void SetupComponents();

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

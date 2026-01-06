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
	// Sets default values for this actor's properties
	AProjectileTG();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class UShapeComponent* CollisionShape; // Root

	UPROPERTY(EditAnywhere)
	TSubclassOf<UShapeComponent> CollisionShapeType;

	UPROPERTY(EditDefaultsOnly)
	class UMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMeshComponent> ProjectileMeshType;

	UPROPERTY(EditDefaultsOnly)
	class UArrowComponent* ProjectileDirectionArrow;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraComponent* ProjectileFX;
	
	//UPROPERTY(EditAnywhere)
	//TSubclassOf<UNiagaraComponent> ProjectileFXType;

	UPROPERTY(EditDefaultsOnly)
	class UNiagaraSystem* ImpactParticles;
	
	UPROPERTY(EditDefaultsOnly)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UProjectileMovementComponent> ProjectileMovementType;

	UPROPERTY(EditDefaultsOnly)
	float BaseDamage{10.f};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageType> DamageType;
	
protected:
	virtual void InitComponents();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

private:
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

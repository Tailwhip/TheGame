// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorsSpawnerTG.generated.h"

UCLASS(Abstract)
class THEGAME_API AActorsSpawnerTG : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorsSpawnerTG();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category="Actors to spawn")
	TSubclassOf<class AActorTG> ActorType;
	
	UPROPERTY(EditAnywhere, Category="Actors to spawn")
	TSubclassOf<class AActorsPoolTG> ActorsPoolType;

	AActorsPoolTG* ActorsPool{nullptr};

	void SpawnFromPool(const FTransform& Transform);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};

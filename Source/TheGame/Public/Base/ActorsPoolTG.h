// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorsPoolTG.generated.h"


UCLASS(Abstract)
class THEGAME_API AActorsPoolTG : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorsPoolTG();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditDefaultsOnly, Category="Actors to spawn")
	uint16 NumberOfActorsToSpawn{0};

	class AActorTG* GetActorFromPool();
	
	TArray<class AActorTG*> Actors;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
};

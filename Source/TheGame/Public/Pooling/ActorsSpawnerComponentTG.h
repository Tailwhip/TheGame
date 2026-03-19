// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Pooling/ActorsPoolTG.h"
#include "ActorsSpawnerComponentTG.generated.h"


class AActorTG;

UCLASS( BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent) )
class THEGAME_API UActorsSpawnerComponentTG : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActorsSpawnerComponentTG();
	// Called every frame
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	void LoadActors();
	void SpawnActor(
		FTransform const& Transform,
		const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters());

	UPROPERTY(EditDefaultsOnly, Category = "SpawnParametersTG")
	TSubclassOf<AActorTG> ActorClass;

	UPROPERTY(EditDefaultsOnly, Category = "SpawnParametersTG")
	uint32 NumOfActors;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
private:
	UPROPERTY()
	FActorsPoolTG ActorsPool;
};

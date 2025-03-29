// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorTG.generated.h"

UCLASS(Abstract)
class THEGAME_API AActorTG : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorTG();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetInUse(bool value);
	bool IsInUse();

	void Activate(const FTransform& Transform);
	void Deactivate();
		
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	bool InUse{false};
	
};

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
	void Tick(float DeltaTime) override;

	void SetInUse(bool value);
	bool IsInUse();

	virtual void Activate();
	virtual void Deactivate();
	virtual void ResetMovement(FTransform const& Transform) {};
	
	UPROPERTY(EditDefaultsOnly, Category = "ActorTG")
	float TimeToLive;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	bool bInUse;
	float TimeToLiveCounter;

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/ActorTG.h"
#include "TheGame/TheGame.h"


// Sets default values
AActorTG::AActorTG():
	Super(),
	TimeToLive{0.f},
	bInUse{false},
	TimeToLiveCounter{0.f}
{
	TRACE("")
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AActorTG::BeginPlay()
{
	Super::BeginPlay();
	TimeToLiveCounter = TimeToLive;
}

// Called every frame
void AActorTG::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TimeToLiveCounter > 0)
	{
		TimeToLiveCounter -= DeltaTime;
		if (TimeToLiveCounter < 0)
		{
			TRACE("TimeToLive expired. Deactivating actor %s", *this->GetName())
			Deactivate();
		}
	}
}

void AActorTG::SetInUse(bool value)
{
	bInUse = value;
}

bool AActorTG::IsInUse()
{
	return bInUse;
}

void AActorTG::Activate()
{
	TRACE("Activating actor %s", *this->GetName())
	this->SetActorEnableCollision(true);
	this->SetActorHiddenInGame(false);
	//this->SetHidden(false);
	this->SetActorTickEnabled(true);
	bInUse = true;
}

void AActorTG::Deactivate()
{
	TRACE("Deactivating actor %s", *this->GetName())
	this->SetActorEnableCollision(false);
	this->SetActorHiddenInGame(true);
	//this->SetHidden(true);
	this->SetActorTickEnabled(false);
	bInUse = false;
}


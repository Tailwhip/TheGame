// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/ActorsPoolTG.h"
#include "Base/ActorTG.h"
#include "TheGame/TheGame.h"


// Sets default values
AActorsPoolTG::AActorsPoolTG()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

AActorTG* AActorsPoolTG::GetActorFromPool()
{
	TRACE("")
	for (auto a : Actors)
	{
		if (!a->IsInUse())
		{
			TRACE("Returning ActorTG")
			return a;
		}
	}
	return nullptr;
}

// Called when the game starts or when spawned
void AActorsPoolTG::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AActorsPoolTG::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


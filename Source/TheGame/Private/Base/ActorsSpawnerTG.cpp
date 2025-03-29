// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorsSpawnerTG.h"
#include "Base/ActorTG.h"
#include "Base/ActorsPoolTG.h"
#include "TheGame/TheGame.h"


// Sets default values
AActorsSpawnerTG::AActorsSpawnerTG()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AActorsSpawnerTG::SpawnFromPool(const FTransform& Transform)
{
	TRACE("")
	AActorTG* Actor = ActorsPool->GetActorFromPool();
	Actor->Activate(Transform);
}

// Called when the game starts or when spawned
void AActorsSpawnerTG::BeginPlay()
{
	TRACE("")
	Super::BeginPlay();
	if (!GetWorld())
	{
		TRACEERROR("GetWorld is nullptr!")
		return;
	}
		
	ActorsPool = GetWorld()->SpawnActor<AActorsPoolTG>(
		ActorsPoolType,
		FVector(0.f, 0.f, 0.f),
		FRotator(0.f, 0.f, 0.f));
	TRACE("Actors Pool Created")
	if (ActorsPool)
	{
		FActorSpawnParameters SpawnParams;
		// SpawnParams.Instigator = ThisCharacter;
		for (uint16 i=0; i < ActorsPool->NumberOfActorsToSpawn; i++)
		{
			auto a = GetWorld()->SpawnActor<AActorTG>(
				ActorType,
				FVector(0.f, 0.f, 0.f),
				FRotator(0.f, 0.f, 0.f),
				SpawnParams);
			ActorsPool->Actors.Emplace(a);
			TRACE("Actor %d Created", i)
		}
	}
}

// Called every frame
void AActorsSpawnerTG::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


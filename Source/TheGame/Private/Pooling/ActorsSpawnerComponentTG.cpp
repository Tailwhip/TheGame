// Fill out your copyright notice in the Description page of Project Settings.


#include "Pooling/ActorsSpawnerComponentTG.h"
#include "Base/ActorTG.h"
#include "TheGame/TheGame.h"


// Sets default values for this component's properties
UActorsSpawnerComponentTG::UActorsSpawnerComponentTG():
	NumOfActors{0}
{
	TRACE("Initialize Actors Spawner with size of %d", NumOfActors);
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

// Called when the game starts
void UActorsSpawnerComponentTG::BeginPlay()
{
	TRACE("BeginPlay Actors Spawner with size of %d", NumOfActors);
	Super::BeginPlay();
	ActorsPool = FActorsPoolTG(NumOfActors);
	LoadActors();
}

void UActorsSpawnerComponentTG::LoadActors()
{
	TRACE("")
	UWorld* World = nullptr;
	if (AActor* Owner = GetOwner())
	{
		World = Owner->GetWorld();
		if (!World)
		{
			TRACEERROR("GetWorld is nullptr!")
			return;
		}
	}
	else
	{
		TRACEERROR("GetOwner is nullptr!")
		return;
	}
	if (ActorClass)
	{
		TRACE("Loading %d actors to memory...", NumOfActors)
		FActorSpawnParameters SpawnParameters;
		// SpawnParams.Instigator = ThisCharacter;
		for (uint32 i = 0; i < NumOfActors; ++i)
		{
			TRACE("Loading actor %d", i)
			auto Actor = World->SpawnActor<AActorTG>(
				ActorClass,
				FVector(0.f, 0.f, 0.f),
				FRotator(0.f, 0.f, 0.f),
				SpawnParameters);
			Actor->Deactivate();
			ActorsPool.Add(Actor);
			TRACE("Actor %d has been created", i)
		}
	}
	else
	{
		TRACEERROR("ActorClass is not set up!")
		return;
	}
	TRACE("Loaded actors successfully!")
}

void UActorsSpawnerComponentTG::SpawnActor(
	FTransform const& Transform,
	const FActorSpawnParameters& SpawnParameters)
{
	auto Actor = ActorsPool.GetNext();
	if (!Actor)
	{
		TRACEERROR("No more actors in pool!")
		return;
	}
	TRACE("Spawning actor: %s with Location: [%f %f %f] and Rotator: [%f, %f, %f]", 
		*Actor->GetName(), 
		Transform.GetLocation().X,
		Transform.GetLocation().Y,
		Transform.GetLocation().Z,
		Transform.GetRotation().Rotator().Pitch,
		Transform.GetRotation().Rotator().Yaw,
		Transform.GetRotation().Rotator().Roll)

	//// set transform
	//Actor->SetActorLocationAndRotation(Transform.GetLocation(), Transform.GetRotation());

	// set ownership
	Actor->SetOwner(SpawnParameters.Owner);
	Actor->SetInstigator(SpawnParameters.Instigator);

	// activate
	Actor->Activate();
	//Actor->SetActorTransform(Transform);
	Actor->SetActorLocationAndRotation(
		Transform.GetLocation(),
		Transform.GetRotation().Rotator());
	Actor->ResetMovement(Transform);
}

// Called every frame
void UActorsSpawnerComponentTG::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


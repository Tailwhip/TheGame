// Fill out your copyright notice in the Description page of Project Settings.


#include "Base/ActorTG.h"
#include "TheGame/TheGame.h"


// Sets default values
AActorTG::AActorTG()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Deactivate();
}

// Called when the game starts or when spawned
void AActorTG::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActorTG::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AActorTG::SetInUse(bool value)
{
	InUse = value;
}

bool AActorTG::IsInUse()
{
	return InUse;
}

void AActorTG::Activate(const FTransform& Transform)
{
	TRACE("")
	this->SetActorTransform(Transform);
	this->SetActorEnableCollision(true);
	this->SetHidden(true);
	this->SetActorTickEnabled(true);
	InUse = true;
}

void AActorTG::Deactivate()
{
	TRACE("")
	this->SetActorEnableCollision(false);
	this->SetHidden(false);
	this->SetActorTickEnabled(false);
	InUse = false;
}


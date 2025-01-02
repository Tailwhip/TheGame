// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileTG.h"
#include "TheGame/TheGame.h"

// Sets default values
AProjectileTG::AProjectileTG()
{
	TRACE("")
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AProjectileTG::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AProjectileTG::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


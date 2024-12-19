// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterTG.h"

#include "GameFramework/PawnMovementComponent.h"

// Sets default values
ACharacterTG::ACharacterTG()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACharacterTG::BeginPlay()
{
	Super::BeginPlay();
	if (GetMovementComponent())
	{
		GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
		GetMovementComponent()->GetNavAgentPropertiesRef().bCanFly = true;
	}
}

// Called every frame
void ACharacterTG::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACharacterTG::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


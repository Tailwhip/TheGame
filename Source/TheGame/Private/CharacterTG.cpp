// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterTG.h"
#include "TheGame/TheGame.h"
#include "GameFramework/PawnMovementComponent.h"
// #include "UniversalObjectLocators/UniversalObjectLocatorUtils.h"

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
	Camera = NewObject<UCameraComponent>(this, "CameraTG");
	// auto CharacterLocation = GetOwner()->GetActorTransform().GetLocation();
	if (Camera)
	{
		Camera->RegisterComponent();
		Camera->AttachToComponent(RootComponent.Get(), FAttachmentTransformRules::KeepRelativeTransform);
		Camera->CreationMethod = EComponentCreationMethod::Instance;
		Camera->bUsePawnControlRotation = true;
		Camera->SetRelativeLocation(FVector(-382.0, 0.0,102.0));
	}
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

void ACharacterTG::ShootProjectile() const
{
	TRACE("")
	GetWorld()->SpawnActor<AProjectileTG>(ProjectileClass, Camera->GetComponentTransform());
}


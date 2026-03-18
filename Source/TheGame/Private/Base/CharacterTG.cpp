// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterTG.h"
#include "TheGame/TheGame.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Actors/DroneHudTG.h"
#include "Pooling/ActorsSpawnerComponentTG.h"
#include "Base/PlayerControllerTG.h"
#include "GameFramework/SpringArmComponent.h"

// #include "UniversalObjectLocators/UniversalObjectLocatorUtils.h"

// Sets default values
ACharacterTG::ACharacterTG():
	Super()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TRACE("Setting up a camera...")
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(false); // Rotate with character
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
	CameraBoom->SetRelativeLocation(FVector(0.f, 50.f,100.f));
	CameraBoom->bDoCollisionTest = false; // Do not collide with environment
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = true; // Enables moves up and down with camera
		
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->AttachToComponent(RootComponent.Get(),
		FAttachmentTransformRules::KeepRelativeTransform);
	ProjectileSpawnPoint->SetRelativeLocation(FVector(300.f, 0.f,0.f));

	GetMesh()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	GetMesh()->SetNotifyRigidBodyCollision(false);

	GetCapsuleComponent()->SetNotifyRigidBodyCollision(false); // Simulation Generates Hit Events
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	ProjectileSpawnerComponent =
		CreateDefaultSubobject<UActorsSpawnerComponentTG>(TEXT("ProjectileSpawner"));

	if (IsLocallyControlled())
	{
		PlayerController = CreateDefaultSubobject<APlayerControllerTG>(TEXT("Camera"));
	}
	
	DroneHud = CreateWidget<UDroneHudTG>(PlayerController, DroneHudClass);
	if (DroneHud) DroneHud->AddToPlayerScreen();

	ThisCharacter = this;
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

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ACharacterTG::BeginOverlap);
	TRACE("BeginOverlap has been binded")
}

void ACharacterTG::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (DroneHud)
	{
		DroneHud->RemoveFromParent();
		DroneHud = nullptr;
	}
}

// Called every frame
void ACharacterTG::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void ACharacterTG::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TRACE("OverlappedComponent: %s, OtherComponent: %s",
		*OverlappedComponent->GetName(), *OtherComponent->GetName())
}

// Called to bind functionality to input
void ACharacterTG::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACharacterTG::ShootProjectile() const
{
	TRACE("")

	if (!GetWorld())
	{
		TRACEERROR("Could not get the world!")
		return;
	}

	// 1. Camera view (UE 5.7-safe)
	FVector CameraLocation;
	FRotator CameraRotation;
	GetActorEyesViewPoint(CameraLocation, CameraRotation);

	// 2. Camera trace
	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + (CameraRotation.Vector() * TraceDistance);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.bTraceComplex = true;

	FVector AimPoint = TraceEnd;

	if (GetWorld()->LineTraceSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		Params))
	{
		TRACE("AimPoint taken from hit!")
		AimPoint = Hit.ImpactPoint;
	}

	// 3. Muzzle socket
	FVector MuzzleLocation = GetMesh()->GetSocketLocation(TEXT("Muzzle"));

	// 4. Direction correction
	FVector ShotDirection = (AimPoint - MuzzleLocation).GetSafeNormal();
	FRotator SpawnRotation = ShotDirection.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = ThisCharacter;
	SpawnParams.Owner = ThisCharacter;
	if (!ThisCharacter)
	{
		TRACEERROR("ThisCharacter not exists!")
		return;
	}
	// TODO: Setup SpawnParams like SpawnCollisionHandlingOverride or OverrideLevel

	SpawnRotation.Pitch += 5.f;
	SpawnRotation.Yaw += 1.f;
	ProjectileSpawnerComponent->SpawnActor(
		FTransform(SpawnRotation, MuzzleLocation),
		SpawnParams);
	
	TRACE("Created projectile")
}

float ACharacterTG::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	TRACE("")
	HealthPoints -= DamageAmount;
	if (0 >= HealthPoints)
	{
		TRACE("I'm dead!")
		Destroy();
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

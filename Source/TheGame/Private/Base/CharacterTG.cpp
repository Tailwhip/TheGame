// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterTG.h"
#include "TheGame/TheGame.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Actors/DroneHudTG.h"
#include "PlayerControllerTG.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"

// #include "UniversalObjectLocators/UniversalObjectLocatorUtils.h"

// Sets default values
ACharacterTG::ACharacterTG() : Super()
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
	
	// Camera->RegisterComponent();
	// Camera->AttachToComponent(RootComponent.Get(), FAttachmentTransformRules::KeepRelativeTransform);
	// Camera->CreationMethod = EComponentCreationMethod::Instance;
	// Camera->SetRelativeLocation(FVector(-382.0, 0.0,102.0));
	
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ProjectileSpawnPoint"));
	ProjectileSpawnPoint->AttachToComponent(RootComponent.Get(),
		FAttachmentTransformRules::KeepRelativeTransform);
	ProjectileSpawnPoint->SetRelativeLocation(FVector(200.f, 0.f,0.f));

	GetMesh()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName(TEXT("NoCollision"));
	GetMesh()->SetNotifyRigidBodyCollision(false);

	GetCapsuleComponent()->SetNotifyRigidBodyCollision(false); // Simulation Generates Hit Events
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	// GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	// GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Overlap);



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

// Called to bind functionality to input
void ACharacterTG::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACharacterTG::ShootProjectile() const
{
	TRACE("")

	if (!ProjectileClass)
	{
		TRACEERROR("Projectile class is missing in blueprint!")
		return;
	}
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
	// FVector SpawnLocation = Camera->GetComponentLocation();
	// SpawnLocation.X += 200.f;
	// SpawnLocation.Z -= 50.f;
	//FRotator SpawnRotation = CameraBoom->GetComponentRotation();
	SpawnRotation.Pitch += 5.f;
	SpawnRotation.Yaw += 1.f;
	GetWorld()->SpawnActor<AProjectileTG>(
		ProjectileClass,
		MuzzleLocation,
		SpawnRotation,
		SpawnParams
	);
	// GetWorld()->SpawnActor<AProjectileTG>(ProjectileClass, Camera->GetComponentTransform());
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

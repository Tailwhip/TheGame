// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterTG.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "TheGame/TheGame.h"
#include "Base/Controller/AIControllerTG.h"
#include "Base/Controller/PlayerControllerTG.h"
#include "Pooling/ActorsSpawnerComponentTG.h"
#include "PythonCommunication/MessageDispatcherTG.h"
#include "PythonCommunication/PythonCommunicationComponentTG.h"
#include "UI/CharacterHUDWidgetTG.h"


ACharacterTG::ACharacterTG():
	Super(),
	TraceDistance{ 100000.f },
	HealthPoints{ 100.0 },
	AttackPower{ 10.0 },
	bIsDestroyable{ true },
	bIsAI{ false },
	bDidDamage{ false },
	bDidKill{ false },
	bWasHit{ false },
	bWasKilled{ false }
{
	// Set this character to call Tick() every frame. 
	// You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

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
		
	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(
		TEXT("ProjectileSpawnPoint"));
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

	PythonCommunicationComponent =
		CreateDefaultSubobject<UPythonCommunicationComponentTG>(TEXT("PythonCommunication"));

	AIControllerClass = AAIControllerTG::StaticClass();
	AutoPossessAI = EAutoPossessAI::Disabled;

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
	TRACE("BeginOverlap has been binded");

	//SpawnDefaultController();
	if (bIsAI)
	{
		if (!GetController())
		{
			if (AIControllerClass)
			{
				AAIController* AI = GetWorld()->SpawnActor<AAIController>(AIControllerClass);
				if (AI)
				{
					AI->Possess(this);
					TRACE("Character %s is AI possessed", *ThisCharacter->GetName());
				}
				else
				{
					TRACEWARN("AIController is null!");
				}
			}
			else
			{
				TRACEWARN("AIControllerClass is null!");
			}
		}
		else
		{
			TRACEWARN("Character %s already has controller!", *ThisCharacter->GetName());
		}
	}
	else
	{
		APlayerControllerTG* playerController = Cast<APlayerControllerTG>(GetController());
		if (playerController)
		{
			if (HUDClass)
			{
				HUDWidget = CreateWidget<UCharacterHUDWidgetTG>(playerController, HUDClass);
				if (HUDWidget) HUDWidget->AddToPlayerScreen();
				else TRACEWARN("No HUDWidget has been set!");
			}
			else
			{
				TRACEWARN("No HUDClass has been set!");
			}
		}
		else
		{
			TRACEWARN("PlayerController is null!");
		}
	}
	MaxHealth = HealthPoints;
	OnHealthChanged(MaxHealth);
}

void ACharacterTG::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
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
		TRACEERROR("Could not get the world!");
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
		TRACE("AimPoint taken from hit!");
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
		TRACEERROR("ThisCharacter not exists!");
		return;
	}
	// TODO: Setup SpawnParams like SpawnCollisionHandlingOverride or OverrideLevel

	SpawnRotation.Pitch += 5.f;
	SpawnRotation.Yaw += 1.f;
	ProjectileSpawnerComponent->SpawnActor(
		FTransform(SpawnRotation, MuzzleLocation),
		SpawnParams);
	
	TRACE("Created projectile");
}

void ACharacterTG::OnHealthChanged(float NewHealth)
{
	TRACE("");
	if (HUDWidget)
	{
		HUDWidget->SetHealth(NewHealth, MaxHealth);
	}
	bWasHit = true;
}

void ACharacterTG::OnDeath()
{
	TRACE("%s character is dead!", *ThisCharacter->GetName());
	bWasKilled = true;
	Destroy();
}

void ACharacterTG::OnHit(AActor* OtherActor)
{
	TRACE("%s character has hit %s",
		*ThisCharacter->GetName(),
		*OtherActor->GetName());
	bDidDamage = true;
}

void ACharacterTG::OnKill()
{
	TRACE("%s character has killed someone", *ThisCharacter->GetName());
	bDidKill = true;
}

float ACharacterTG::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser)
{
	TRACE("%s is taking damage: %f", *ThisCharacter->GetName(), DamageAmount);
	if (!bIsDestroyable)
		TRACE("%s character is immortal!", *ThisCharacter->GetName())
	else
	{
		HealthPoints -= DamageAmount;
		OnHealthChanged(HealthPoints);
		if (0 >= HealthPoints) OnDeath();
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

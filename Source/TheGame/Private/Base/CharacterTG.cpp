// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterTG.h"
#include "TheGame/TheGame.h"
#include "GameFramework/PawnMovementComponent.h"
#include "DroneHudTG.h"
#include "PlayerControllerTG.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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

	if (IsLocallyControlled())
	{
		TRACE("Is locally controlled")
		PlayerController = CreateDefaultSubobject<APlayerControllerTG>(TEXT("LocalPlayerController"));
	}
	else
		TRACE("Is globally controlled")

	DroneHud = CreateWidget<UDroneHudTG>(PlayerController, DroneHudClass);
	// check(DroneHud);
	if (DroneHud) 
		DroneHud->AddToPlayerScreen();
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
	if (ProjectileClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = ThisCharacter;
		if (ThisCharacter)
			TRACE("ThisCharacter exists")
		else
		{
			TRACEERROR("ThisCharacter not exists!")
		}

		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		if (!PC) return;

		GetWorld()->SpawnActor<AProjectileTG>(
			ProjectileClass,
			ProjectileSpawnPoint->GetComponentLocation(),
			ProjectileSpawnPoint->GetComponentRotation(),
			SpawnParams);
		TRACE("Projectile spawned")
		// TODO: Setup SpawnParams like SpawnCollisionHandlingOverride or OverrideLevel
		// FVector SpawnLocation = Camera->GetComponentLocation();
		// SpawnLocation.X += 200.f;
		// SpawnLocation.Z -= 50.f;
		//FRotator SpawnRotation = ProjectileSpawnPoint->GetComponentRotation();
		//SpawnRotation.Yaw += 50.f;

		FVector WorldLocation, WorldDirection;

		// Get screen size
		int32 ViewportSizeX, ViewportSizeY;
		PC->GetViewportSize(ViewportSizeX, ViewportSizeY);

		// Convert screen center to world direction
		FVector2D ScreenCenter(ViewportSizeX / 2.0f, ViewportSizeY / 2.0f);
		TRACE("Here1")
		if (PC->DeprojectScreenPositionToWorld(ScreenCenter.X, ScreenCenter.Y, WorldLocation, WorldDirection))
		{
			TRACE("Here2")
			// Get the starting location (projectile's current position)
			FVector StartLocation = GetActorLocation();
			TRACE("Here3")
			// Trace a line in the direction to get a hit location (optional)
			FVector EndLocation = WorldLocation + (WorldDirection * 10000.0f); // long distance

			FVector ShootDirection = (EndLocation - StartLocation).GetSafeNormal();
			TRACE("Here4")
			// Apply velocity
			if (UProjectileMovementComponent* ProjectileMovement = FindComponentByClass<UProjectileMovementComponent>())
			{
				TRACE("Here5")
				ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
			}
		}
		TRACE("Here6")
		TRACE("Created projectile")
	}
	else
	{
		TRACEERROR("Projectile class is missing in the blueprint!")
	}
}

float ACharacterTG::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	TRACE("")
	HP -= DamageAmount;
	if (0 >= HP)
	{
		TRACE("I'm dead!")
		Destroy();
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

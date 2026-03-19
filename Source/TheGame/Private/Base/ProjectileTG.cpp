// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileTG.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/GameModeBase.h"
#include "Components/SphereComponent.h"
#include "Components/MeshComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Tools/UtilsTG.h"
#include "TheGame/TheGame.h"


// Sets default values
AProjectileTG::AProjectileTG():
	Super()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionShape = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollisionShapeTG"));
	// CollisionShape = UtilsTG::CreateDefaultSubobject<UShapeComponent, USphereComponent>(
	// 	this, TEXT("ProjectileCollisionShapeTG"), CollisionShapeType
	// );
	RootComponent = CollisionShape;

	TRACE("CollisionShape (Root) created!")

	//ProjectileMesh = UtilsTG::CreateDefaultSubobject<UMeshComponent, UStaticMeshComponent>(
	//	this, TEXT("ProjectileMeshTG"), ProjectileMeshType
	//);
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshTG"));
	ProjectileMesh->SetupAttachment(RootComponent);

	ProjectileDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileDirectionArrowTG"));
	ProjectileDirectionArrow->SetupAttachment(RootComponent);

	//ProjectileFX = UtilsTG::CreateDefaultSubobject<UNiagaraComponent, UNiagaraComponent>(
	//	this, TEXT("ProjectileEffectTG"), ProjectileFXType
	//);
	ProjectileFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileEffectTG"));
	ProjectileFX->SetupAttachment(RootComponent);

	ProjectileMovement = UtilsTG::CreateDefaultSubobject<UProjectileMovementComponent, UProjectileMovementComponent>(
		this, TEXT("ProjectileMovementComponentTG"), ProjectileMovementType
	);
	
	TRACE("Components initialized")
}

void AProjectileTG::OnConstruction(const FTransform& InTransform)
{
	Super::OnConstruction(InTransform);
	SetupComponents();
}

void AProjectileTG::SetupComponents()
{
	// Overlap-based collision
	CollisionShape->SetCollisionProfileName(TEXT("Custom"));
	CollisionShape->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionShape->SetGenerateOverlapEvents(true);
	CollisionShape->SetCollisionResponseToAllChannels(ECR_Overlap);

	// Optional but recommended
	CollisionShape->SetCollisionObjectType(ECC_WorldDynamic); // Projectile channel
	TRACE("Projectile collision response to pawn: %d", CollisionShape->GetCollisionResponseToChannel(ECC_Pawn))

	ProjectileMesh->SetGenerateOverlapEvents(false);
	ProjectileMesh->SetCollisionProfileName(TEXT("Custom")); // Breaks presets settings
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	ProjectileMesh->SetUsingAbsoluteLocation(false);
	TRACE("Projectile mesh response to pawn: %d", ProjectileMesh->GetCollisionResponseToChannel(ECC_Pawn))

	ProjectileDirectionArrow->bHiddenInGame = true;

	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 6000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bSweepCollision = true;
	ProjectileMovement->bShouldBounce = false;
	//InitialLifeSpan = 5.0f;

	ProjectileMovement->UpdatedComponent = CollisionShape;
	TRACE("UpdatedComponent = %s", *GetNameSafe(ProjectileMovement->UpdatedComponent));

	// Relevant for multiplayer games
	// bReplicates = true;
	// SetReplicateMovement(true);
}

void AProjectileTG::Activate()
{
	Super::Activate();
	TRACE("Activating projectile...")
	ProjectileMesh->AttachToComponent(
		RootComponent,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale);
}

void AProjectileTG::ResetMovement(FTransform const& Transform)
{
	TRACE("Resetting projectile movement")
	ProjectileMovement->Velocity = 
		Transform.GetRotation().GetForwardVector() * ProjectileMovement->InitialSpeed;
}

// Called every frame
void AProjectileTG::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called when the game starts or when spawned
void AProjectileTG::BeginPlay()
{
	Super::BeginPlay();
	CollisionShape->OnComponentBeginOverlap.AddDynamic(this, &AProjectileTG::BeginOverlap);
	TRACE("BeginOverlap has been binded")
}

void AProjectileTG::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, 
	const FHitResult& SweepResult)
{
	TRACE("OverlappedComponent: %s, OtherComponent: %s",
		*OverlappedComponent->GetName(), *OtherComponent->GetName())
	if (!OtherActor || OtherActor == GetOwner())
	{
		TRACEERROR("No other actor found!")
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactParticles, GetActorLocation());
	OnHit(OtherActor);
	TRACE("Deactivating %s", *this->GetName())
	Super::Deactivate();
}

void AProjectileTG::OnHit(AActor* OtherActor)
{
	AController* PlayerController = nullptr;
	if (GetInstigator())
	{
		PlayerController = GetInstigator()->GetController();
	}
	else
	{
		TRACEERROR("No Instigator found!")
    }
	if (PlayerController)
	{
		TRACE("Applying damage: %f", BaseDamage)
		UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, PlayerController, this, DamageType);
	}
	else
	{
		TRACEERROR("No PlayerController found!")
    }
}

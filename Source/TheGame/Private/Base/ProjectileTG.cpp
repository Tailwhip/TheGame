// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileTG.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/GameModeBase.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Utils/UtilsTG.h"
#include "TheGame/TheGame.h"


// Sets default values
AProjectileTG::AProjectileTG()
{
	TRACE("")
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionShape = UtilsTG::CreateDefaultSubobject<UShapeComponent, USphereComponent>(
		this, TEXT("ProjectileCollisionShapeTG"), CollisionShapeType
	);
	RootComponent = CollisionShape;
	TRACE("CollisionShape (Root) created!")

	InitComponents();
}

void AProjectileTG::InitComponents()
{
	ProjectileMesh = UtilsTG::CreateDefaultSubobject<UMeshComponent, UStaticMeshComponent>(
		this, TEXT("ProjectileMeshTG"), ProjectileMeshType
	);
	ProjectileMesh->SetupAttachment(RootComponent);

	ProjectileDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileDirectionArrowTG"));
	ProjectileDirectionArrow->SetupAttachment(RootComponent);
	ProjectileDirectionArrow->bHiddenInGame = true;

	//ProjectileFX = UtilsTG::CreateDefaultSubobject<UNiagaraComponent, UNiagaraComponent>(
	//	this, TEXT("ProjectileEffectTG"), ProjectileFXType
	//);
	ProjectileFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileEffectTG"));
	ProjectileFX->SetupAttachment(RootComponent);

	ProjectileMovement = UtilsTG::CreateDefaultSubobject<UProjectileMovementComponent, UProjectileMovementComponent>(
		 this, TEXT("ProjectileMovementComponentTG"), ProjectileMovementType
	);
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 6000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	InitialLifeSpan = 5.0f;

	TRACE("Components initialized")
}

// Called when the game starts or when spawned
void AProjectileTG::BeginPlay()
{
	Super::BeginPlay();
	CollisionShape->OnComponentBeginOverlap.AddDynamic(this, &AProjectileTG::BeginOverlap);
}

void AProjectileTG::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void AProjectileTG::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TRACE("OverlappedComponent: %s, OtherComponent: %s",
		*OverlappedComponent->GetName(), *OtherComponent->GetName())
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactParticles, GetActorLocation());
	OnHit(OtherActor);
	Destroy();
}

void AProjectileTG::OnHit(AActor* OtherActor)
{
	APawn* Instig = GetInstigator();
	AController* PlayerController = nullptr;
	if (Instig) PlayerController = Instig->GetController();

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

// Called every frame
void AProjectileTG::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


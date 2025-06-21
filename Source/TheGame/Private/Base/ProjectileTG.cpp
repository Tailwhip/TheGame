// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileTG.h"
#include "TheGame/TheGame.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AProjectileTG::AProjectileTG()
{
	TRACE("")
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMesh = CreateDefaultSubobject<UMeshComponent>(TEXT("ProjectileMeshTG"));
	SetRootComponent(ProjectileMesh);
	CollisionShape = CreateDefaultSubobject<UShapeComponent>(TEXT("ProjectileCollisionShapeTG"));
	// CollisionShape->SetRelativeScale3D(FVector(0.2125f, 0.2125f, 0.2125f));
	// GetRootComponent()->SetupAttachment(CollisionShape);
	// ProjectileDirectionArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ProjectileDirectionArrowTG"));
	// GetRootComponent()->SetupAttachment(ProjectileDirectionArrow);
	// ProjectileFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileEffectTG"));
	// GetRootComponent()->SetupAttachment(ProjectileFX);
	// ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponentTG"));
	// ProjectileMovement->UpdatedComponent = CollisionShape;
	//ProjectileMovement->InitialSpeed = 5000.f;
	//ProjectileMovement->MaxSpeed = 6000.f;
	//ProjectileMovement->bRotationFollowsVelocity = true;
	//ProjectileMovement->bShouldBounce = false;
	// InitialLifeSpan = 5.0f;
	// TRACE("Projectile created")
}

// Called when the game starts or when spawned
void AProjectileTG::BeginPlay()
{
	Super::BeginPlay();
	CollisionShape->OnComponentBeginOverlap.AddDynamic(this, &AProjectileTG::BeginOverlap);
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


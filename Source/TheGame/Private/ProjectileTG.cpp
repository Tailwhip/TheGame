// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileTG.h"
#include "TheGame/TheGame.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AProjectileTG::AProjectileTG()
{
	TRACE("")
	ProjectileFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ProjectileEffect"));
	SetRootComponent(ProjectileFX);
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ProjectileCollisionObject"));
	CollisionSphere->SetupAttachment(ProjectileFX);
	CollisionSphere->SetRelativeScale3D(FVector(0.2125f, 0.2125f, 0.2125f));
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AProjectileTG::BeginPlay()
{
	Super::BeginPlay();
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AProjectileTG::BeginOverlap);
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
		TRACE("Applying damage: %d", BaseDamage)
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


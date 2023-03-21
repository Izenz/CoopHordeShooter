// Fill out your copyright notice in the Description page of Project Settings.

#include "HSExplosiveProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHSExplosiveProjectile::AHSExplosiveProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(0.5f);
	CollisionComp->SetCollisionProfileName("Projectile");

	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	RootComponent = CollisionComp;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 2000.f;
	ProjectileMovement->MaxSpeed = 2000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	ExplosionRadius = 125.0f;
	ExplosionDamage = 15;

}

// Called when the game starts or when spawned
void AHSExplosiveProjectile::BeginPlay()
{
	Super::BeginPlay();
	

	GetWorldTimerManager().ClearTimer(TimerHandle_Explode);
	GetWorldTimerManager().SetTimer(TimerHandle_Explode, this, &AHSExplosiveProjectile::ExplodeProjectile, 1.0f);
}

void AHSExplosiveProjectile::ExplodeProjectile()
{
	DrawDebugSphere(GetWorld(), this->GetActorLocation(), ExplosionRadius, 12, FColor::Yellow, false, 3.0f);
	
	/*
	AActor* Owner = GetOwner();
	TArray<AActor*> IgnoredActors;
	if (Owner)
	{
		IgnoredActors.Add(Owner);
		UGameplayStatics::ApplyRadialDamage(Owner, ExplosionDamage, GetActorLocation(), ExplosionRadius, DamageType, IgnoredActors, this, Owner->GetInstigatorController(), true, ECC_Visibility);
	}
	*/

	if (ExplosionVFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation(), GetActorRotation(), true);
	}

	Destroy();
}

// Called every frame
void AHSExplosiveProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


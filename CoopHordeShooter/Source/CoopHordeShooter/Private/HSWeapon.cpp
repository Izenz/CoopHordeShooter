// Fill out your copyright notice in the Description page of Project Settings.

#include "HSWeapon.h"
#include "CoopHordeShooter.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Components/SkeletalMeshComponent.h"
#include "TimerManager.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draws Debug Lines for Weapons"),
	ECVF_Cheat);

void AHSWeapon::BeginPlay()
{
	Super::BeginPlay();

	ShootCooldown = 60 / Cadency;
}



// Sets default values
AHSWeapon::AHSWeapon()
{
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
}

void AHSWeapon::Shoot()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		FVector TraceEnd = EyeLocation + (ShotDirection * BulletRange);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		QueryParams.bTraceComplex = true;
		QueryParams.bReturnPhysicalMaterial = true;

		FVector BulletTrailEndPoint = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			// Blocking hit, process damage.

			AActor* HitActor = Hit.GetActor();
			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float HitDamage = SurfaceType == SURFACE_FLESHVULNERABLE ? BaseDamage * HeadshotMultiplier : BaseDamage;

			UGameplayStatics::ApplyPointDamage(HitActor, HitDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			
			UParticleSystem* TargetVFX = nullptr;
			switch (SurfaceType)
			{
			case SURFACE_FLESHDEFAULT:
			case SURFACE_FLESHVULNERABLE:
				TargetVFX = FleshImpactVFX;
				break;
			default:
				TargetVFX = DefaultImpactVFX;
				break;
			}

			if (TargetVFX)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TargetVFX, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			BulletTrailEndPoint = Hit.ImpactPoint;
		}

		if(DebugWeaponDrawing > 0)		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);

		PlayShootVFX(BulletTrailEndPoint);

		LastShotTime = GetWorld()->TimeSeconds;
	}


}

void AHSWeapon::StartShooting()
{
	float FirstDelay = FMath::Max(LastShotTime + ShootCooldown - GetWorld()->TimeSeconds, 0.0f);
	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AHSWeapon::Shoot, ShootCooldown, true, FirstDelay);
}

void AHSWeapon::StopShooting()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void AHSWeapon::PlayShootVFX(FVector BulletTrailEndPoint)
{
	if (MuzzleVFX)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleVFX, MeshComp, MuzzleSocketName);
	}

	FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

	if (BulletTrailVFX)
	{
		UParticleSystemComponent* TrailComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletTrailVFX, MuzzleLocation);
		if (TrailComp)
		{
			TrailComp->SetVectorParameter("Target", BulletTrailEndPoint);
		}
	}

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());
		if (PC)
		{
			PC->ClientPlayCameraShake(FireCamShake);
		}
	}
}


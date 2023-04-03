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
#include "Net/UnrealNetwork.h"

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

	SetReplicates(true);
}

void AHSWeapon::Shoot()
{
	if (!HasAuthority())
	{
		ServerShoot();
	}

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

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			// Blocking hit, process damage.

			AActor* HitActor = Hit.GetActor();
			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float HitDamage = SurfaceType == SURFACE_FLESHVULNERABLE ? BaseDamage * HeadshotMultiplier : BaseDamage;

			UGameplayStatics::ApplyPointDamage(HitActor, HitDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
			
			PlayImpactVFX(SurfaceType, Hit.ImpactPoint);

			BulletTrailEndPoint = Hit.ImpactPoint;
		}

		if(DebugWeaponDrawing > 0)		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);

		PlayShootVFX(BulletTrailEndPoint);

		if (HasAuthority())
		{
			HitScanTrace.TraceTo = BulletTrailEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;
		}

		LastShotTime = GetWorld()->TimeSeconds;
	}


}

void AHSWeapon::ServerShoot_Implementation()
{
	Shoot();
}

bool AHSWeapon::ServerShoot_Validate()
{
	return true;
}

void AHSWeapon::OnRep_HitScanTrace()
{
	PlayShootVFX(HitScanTrace.TraceTo);
	PlayImpactVFX(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
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

void AHSWeapon::PlayImpactVFX(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
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
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TargetVFX, ImpactPoint, ShotDirection.Rotation());
	}
}

void AHSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AHSWeapon, HitScanTrace, COND_SkipOwner);
}


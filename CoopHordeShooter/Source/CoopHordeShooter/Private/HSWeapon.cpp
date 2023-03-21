// Fill out your copyright notice in the Description page of Project Settings.

#include "HSWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SkeletalMeshComponent.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(
	TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draws Debug Lines for Weapons"),
	ECVF_Cheat);

// Sets default values
AHSWeapon::AHSWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "Target";
}

// Called when the game starts or when spawned
void AHSWeapon::BeginPlay()
{
	Super::BeginPlay();

}

void AHSWeapon::Shoot()
{
	// Trade the world from pawn eyes to crosshair location

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

		FVector BulletTrailEndPoint = TraceEnd;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECC_Visibility, QueryParams))
		{
			// Blocking hit, process damage.

			AActor* HitActor = Hit.GetActor();

			UGameplayStatics::ApplyPointDamage(HitActor, 20.0f, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			if (ImpactVFX)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
			}

			BulletTrailEndPoint = Hit.ImpactPoint;
		}

		if(DebugWeaponDrawing > 0)		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);

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
	}


}

// Called every frame
void AHSWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


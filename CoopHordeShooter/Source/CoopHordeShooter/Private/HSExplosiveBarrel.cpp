// Fill out your copyright notice in the Description page of Project Settings.

#include "HSExplosiveBarrel.h"
#include "Components/HSHealthComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHSExplosiveBarrel::AHSExplosiveBarrel()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<UHSHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &AHSExplosiveBarrel::OnHealthChanged);

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->Radius = 250;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->bIgnoreOwningActor = true;

	ExplosionForce = 400;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void AHSExplosiveBarrel::OnRep_Exploded()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, GetActorLocation());
	MeshComp->SetMaterial(0, ExplodedMaterial);
}

void AHSExplosiveBarrel::OnHealthChanged(UHSHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (bHasExploded)	return;

	if (Health <= 0.0f)
	{
		bHasExploded = true;
		OnRep_Exploded();

		FVector BoostIntensity = FVector::UpVector * ExplosionForce;
		MeshComp->AddImpulse(BoostIntensity, NAME_None, true);

		// Since other elements might be used for cover, etc meaning that its important to replicate the movement we trigger explosion on the server only
		// If they were trivial to the gameplay this function should be called client side instead of replicating the movement.
		RadialForceComp->FireImpulse();
	}
}

void AHSExplosiveBarrel::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHSExplosiveBarrel, bHasExploded);
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HSExplosiveBarrel.generated.h"

class UHSHealthComponent;
class UStaticMeshComponent;
class URadialForceComponent;
class UParticleSystem;

UCLASS()
class COOPHORDESHOOTER_API AHSExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHSExplosiveBarrel();

protected:

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UHSHealthComponent* HealthComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URadialForceComponent* RadialForceComp;

	UPROPERTY(ReplicatedUsing=OnRep_Exploded)
	bool bHasExploded;

	UFUNCTION()
	void OnRep_Exploded();

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	float ExplosionForce;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UParticleSystem* ExplosionVFX;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UMaterialInterface* ExplodedMaterial;

	UFUNCTION()
	void OnHealthChanged(UHSHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
public:	

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HSExplosiveProjectile.generated.h"

class USphereComponent;
class UParticleSystem;
class UProjectileMovementComponent;
class UDamageType;

UCLASS()
class COOPHORDESHOOTER_API AHSExplosiveProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHSExplosiveProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float ExplosionRadius;
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* ExplosionVFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Projectile")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	FTimerHandle TimerHandle_Explode;

	UFUNCTION()
	void ExplodeProjectile();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};

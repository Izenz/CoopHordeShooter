// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HSWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class UCameraShake;
class USoundCue;

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:

	UPROPERTY()
		TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
		FVector_NetQuantize TraceTo;
};

UCLASS()
class COOPHORDESHOOTER_API AHSWeapon : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHSWeapon();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USkeletalMeshComponent* MeshComp;

	float BulletRange;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float BaseDamage = 20;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float HeadshotMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		float Cadency = 600;	// RPM

	// Bullet spread in DEGREES
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin=0.0f))
		float BulletSpread = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* MuzzleVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* DefaultImpactVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* FleshImpactVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
		UParticleSystem* BulletTrailVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* ShootSFX;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* EnemyHitSFX;

	void PlayShootVFX(FVector BulletTrailEndPoint);

	void PlayImpactVFX(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		TSubclassOf<UCameraShake> FireCamShake;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void Shoot();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerShoot();

	FTimerHandle TimerHandle_TimeBetweenShots;

	float LastShotTime;

	float ShootCooldown;

	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
		FHitScanTrace HitScanTrace;

	UFUNCTION()
		void OnRep_HitScanTrace();

public:

	void StartShooting();

	void StopShooting();
};

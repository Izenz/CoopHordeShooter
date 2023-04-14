// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HSTrackerBall.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UHSHealthComponent;
class UParticleSystem;
class USoundCue;

UCLASS()
class COOPHORDESHOOTER_API AHSTrackerBall : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHSTrackerBall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;

	UFUNCTION()
	void HandleTakeAnyDamage(UHSHealthComponent* HealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	FVector GetNextPathPoint();

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float MovementSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float RequiredDistanceToTarget;

	UHSHealthComponent* HealthComp;

	// Material, pulsing on damage
	UMaterialInstanceDynamic* MatInst;

	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ExplosionVFX;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float ExplosionDamage;

	bool bHasExploded;

	bool bStartedSelfDestruction;

	FTimerHandle TimerHandle_SelfDamage;

	void DamageSelf();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SelfDamageInterval;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* SelfDestructSFX;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundCue* ExplosionSFX;

	int32 ChargeLevel;

	void OnCheckNearbyBalls();

	FTimerHandle TimerHandle_RefreshPath;

	void RefreshPath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};

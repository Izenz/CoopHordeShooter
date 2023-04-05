// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HSPickupActor.generated.h"

class USphereComponent;
class UDecalComponent;
class AHSPowerupActor;

UCLASS()
class COOPHORDESHOOTER_API AHSPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHSPickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComp;

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	TSubclassOf<AHSPowerupActor> PowerupClass;

	AHSPowerupActor* PowerupInstance;

	void Respawn();

	UPROPERTY(EditInstanceOnly, Category = "PickupActor")
	float RespawnCooldown;

	FTimerHandle TimerHandle_RespawnTimer;

public:	

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};

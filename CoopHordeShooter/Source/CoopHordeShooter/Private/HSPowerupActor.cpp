// Fill out your copyright notice in the Description page of Project Settings.

#include "HSPowerupActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHSPowerupActor::AHSPowerupActor()
{
	PowerupTickInterval = 0.0f;
	TotalNumOfTicks = 0.0f;

	bIsPowerupActive = false;

	SetReplicates(true);
}

void AHSPowerupActor::OnPowerupTick()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TicksProcessed >= TotalNumOfTicks)
	{
		OnExpiration();

		bIsPowerupActive = false;
		OnRep_PowerupActive();
		

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void AHSPowerupActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerupActive);
}

void AHSPowerupActor::ActivatePowerup()
{
	OnActivation();

	bIsPowerupActive = true;
	OnRep_PowerupActive();

	if (PowerupTickInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &AHSPowerupActor::OnPowerupTick, PowerupTickInterval, true);
	}
	else
	{
		OnPowerupTick();
	}
}

void AHSPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHSPowerupActor, bIsPowerupActive);
}



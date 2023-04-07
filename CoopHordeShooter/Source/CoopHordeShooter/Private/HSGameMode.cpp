// Fill out your copyright notice in the Description page of Project Settings.

#include "HSGameMode.h"
#include "HSGameState.h"
#include "Components/HSHealthComponent.h"
#include "TimerManager.h"
#include "EngineUtils.h"


AHSGameMode::AHSGameMode()
{
	WaveCount = 0;
	TimeBetweenWaves = 2.0f;

	GameStateClass = AHSGameState::StaticClass();

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void AHSGameMode::StartWave()
{
	WaveCount++;
	NumOfEnemiesPerWave = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &AHSGameMode::SpawnEnemyTimerElapsed, 1.0f, true, 0.0f);

	SetWaveState(EWaveState::WaveInProgress);
}

void AHSGameMode::SpawnEnemyTimerElapsed()
{
	SpawnNewEnemy();

	NumOfEnemiesPerWave--;

	if (NumOfEnemiesPerWave <= 0)
	{
		EndWave();
	}
}

void AHSGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	SetWaveState(EWaveState::WaitingToComplete);
}

void AHSGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AHSGameMode::StartWave, TimeBetweenWaves, false);

	SetWaveState(EWaveState::PreparingWave);
}

void AHSGameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NumOfEnemiesPerWave > 0 || bIsPreparingForWave)	return;

	bool bIsAnyEnemyAlive = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; It++)
	{
		APawn* MyPawn = It->Get();
		if (MyPawn == nullptr || MyPawn->IsPlayerControlled())
		{
			continue;
		}

		UHSHealthComponent* HealthComp = Cast<UHSHealthComponent>(MyPawn->GetComponentByClass(UHSHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			bIsAnyEnemyAlive = true;
			break;
		}
	}

	if (!bIsAnyEnemyAlive)
	{
		SetWaveState(EWaveState::WaveComplete);
		PrepareForNextWave();
	}
}

void AHSGameMode::CheckForAlivePlayers()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			UHSHealthComponent* HealthComp = Cast<UHSHealthComponent>(MyPawn->GetComponentByClass(UHSHealthComponent::StaticClass()));
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.0f)
			{
				return;
			}
		}
	}
	
	GameOver();
}

void AHSGameMode::GameOver()
{
	EndWave();

	SetWaveState(EWaveState::GameOver);

	UE_LOG(LogTemp, Log, TEXT("Game over! No alive players remaining."));
}

void AHSGameMode::SetWaveState(EWaveState NewState)
{
	AHSGameState* GS = GetGameState<AHSGameState>();
	if (ensureAlways(GS))
	{
		GS->SetWaveState(NewState);
	}
}

void AHSGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void AHSGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
	CheckForAlivePlayers();
}

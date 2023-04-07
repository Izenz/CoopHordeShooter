// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HSGameMode.generated.h"

enum class EWaveState : uint8;

UCLASS()
class COOPHORDESHOOTER_API AHSGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:

	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	int32 NumOfEnemiesPerWave;

	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewEnemy();
	
	void SpawnEnemyTimerElapsed();

	void StartWave();

	void EndWave();

	void PrepareForNextWave();

	void CheckWaveState();

	void CheckForAlivePlayers();

	void GameOver();

	void SetWaveState(EWaveState NewState);
	
public:

	AHSGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

};

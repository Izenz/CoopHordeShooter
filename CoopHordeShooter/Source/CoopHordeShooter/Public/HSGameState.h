// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HSGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	// When in main menu. Awaits for event to start preparing waves
	GamePaused,

	// Warm up phase
	PreparingWave,

	// Spawn bots phase
	WaveInProgress,

	// Non-spawn phase. Waits for enemies or player to be wiped out.
	WaitingToComplete,

	// Enemies wiped out. 
	WaveComplete,

	// Player wiped out. Awaits for event to restart game.
	GameOver,
};


UCLASS()
class COOPHORDESHOOTER_API AHSGameState : public AGameStateBase
{
	GENERATED_BODY()
	
protected:

	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_WaveState, Category = "GameState")
	EWaveState WaveState;

public:

	void SetWaveState(EWaveState NewState);
};

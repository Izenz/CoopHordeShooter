// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HSPlayerState.generated.h"

UCLASS()
class COOPHORDESHOOTER_API AHSPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddScore(float ScoreDelta);

	UFUNCTION()
	virtual void OnRep_Score() override;
};

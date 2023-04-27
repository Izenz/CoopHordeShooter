// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class COOPHORDESHOOTER_API AHSPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
	void UpdatePlayerScore(float NewScore);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
	void BindHealthCompToIndicator(UHSHealthComponent* HealthComp);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
	void OnPlayerKilled(UHSHealthComponent* HealthComp);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player")
	void DrawDeathScreen();
};

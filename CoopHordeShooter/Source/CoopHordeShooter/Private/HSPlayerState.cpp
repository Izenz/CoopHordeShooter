// Fill out your copyright notice in the Description page of Project Settings.

#include "HSPlayerState.h"
#include "HSPlayerController.h"



void AHSPlayerState::AddScore(float ScoreDelta)
{
	Score += ScoreDelta;
	OnRep_Score();
}

void AHSPlayerState::OnRep_Score()
{
	// Update HUD
	if (AHSPlayerController* AC = Cast<AHSPlayerController>(GetOwner()))
	{
		AC->UpdatePlayerScore(Score);
	}
	
}


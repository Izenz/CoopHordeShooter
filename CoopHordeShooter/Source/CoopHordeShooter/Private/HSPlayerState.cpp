// Fill out your copyright notice in the Description page of Project Settings.

#include "HSPlayerState.h"
#include "HSCharacter.h"



void AHSPlayerState::AddScore(float ScoreDelta)
{
	Score += ScoreDelta;
	OnRep_Score();
}

void AHSPlayerState::OnRep_Score()
{
	// Actualizar HUD
	if (APlayerController* AC = Cast<APlayerController>(GetOwner()))
	{
		if (AHSCharacter* MyChar = Cast<AHSCharacter>(AC->GetPawn()))
		{
			MyChar->UpdatePlayerScore(Score);
		}
	}
	
}


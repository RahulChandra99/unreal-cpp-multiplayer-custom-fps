// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameMode.h"

#include "CustomGameState.h"

void ACustomGameMode::StartMatch()
{
	Super::StartMatch();

	ACustomGameState* GS = GetGameState<ACustomGameState>();
	if (GS)
	{
		GS->RemainingTime = MatchLength;
	}

	GetWorld()->GetTimerManager().SetTimer(
		MatchTimerHandle,
		this,
		&ACustomGameMode::TickMatchTimer,
		1.0f,
		true
	);
}

void ACustomGameMode::TickMatchTimer()
{
	ACustomGameState* GS = GetGameState<ACustomGameState>();
	if (!GS) return;

	GS->RemainingTime--;

	if (GS->RemainingTime <= 0)
	{
		EndMatch();
		GetWorld()->GetTimerManager().ClearTimer(MatchTimerHandle);
	}
}

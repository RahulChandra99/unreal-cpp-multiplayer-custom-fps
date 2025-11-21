// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "CustomGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CUSTOMFPS_API ACustomGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void StartMatch() override;

protected:
	void TickMatchTimer();

	FTimerHandle MatchTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Match")
	int32 MatchLength = 600; // 10 minutes
	
};

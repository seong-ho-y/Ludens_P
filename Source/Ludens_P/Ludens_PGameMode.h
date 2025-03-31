// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Ludens_PGameMode.generated.h"



UCLASS(minimalapi)
class ALudens_PGameMode : public AGameModeBase
{
	GENERATED_BODY()

	// Ludens_PGameMode.h
	UPROPERTY()
	TArray<AActor*> UsedStartSpots;


	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override; //ChoosePlayerStart


public:
	ALudens_PGameMode();
};


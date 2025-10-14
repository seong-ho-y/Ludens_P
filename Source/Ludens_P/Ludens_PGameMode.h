// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EEnemyColor.h"
#include "Ludens_PGameMode.generated.h"


enum class EEnemyColor : uint8;

UCLASS(minimalapi)
class ALudens_PGameMode : public AGameMode
{
	GENERATED_BODY()

	//플레이어 시작 위치 받기
	UPROPERTY()
	TArray<AActor*> UsedStartSpots;

	//적 생성
public:
	ALudens_PGameMode();
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override; //ChoosePlayerStart
	virtual void OnPostLogin(AController* NewPlayer) override;

	//Pawn이 준비된 후에 호출되는 함수
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
	
	void BeginPlay();

protected:
	void AssignColorToPlayer(AController* NewPlayer);
	int32 NumLoggedInPlayers = 0;
	TArray<EEnemyColor> ColorRotation;
};


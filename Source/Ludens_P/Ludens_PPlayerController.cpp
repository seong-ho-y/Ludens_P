// Copyright Epic Games, Inc. All Rights Reserved.


#include "Ludens_PPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnemyPoolManager.h"
#include "EngineUtils.h"
#include "WalkerEnemy.h"
#include "Engine/LocalPlayer.h"

void ALudens_PPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}void ALudens_PPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("SpawnEnemy", IE_Pressed, this, &ALudens_PPlayerController::SpawnEnemyFromPool);
}

void ALudens_PPlayerController::SpawnEnemyFromPool()
{
	for (TActorIterator<AEnemyPoolManager> It(GetWorld()); It; ++It)
	{
		AEnemyPoolManager* Pool = *It;
		if (Pool)
		{
			UE_LOG(LogTemp,Log,TEXT("SpawnEnemyFrompool 호출"));
			// 위치와 회전 지정
			FVector SpawnLoc = FVector(100.f, 100.f, 200.f);
			FRotator SpawnRot = FRotator::ZeroRotator;

			// 스폰 호출
			//Pool->SpawnEnemy(, SpawnLoc, SpawnRot);
		}
	}
}
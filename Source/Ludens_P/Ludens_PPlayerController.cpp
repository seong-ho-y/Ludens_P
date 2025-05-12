// Copyright Epic Games, Inc. All Rights Reserved.


#include "Ludens_PPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnemyPoolManager.h"
#include "EngineUtils.h"
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
			Pool->SpawnEnemy(); // ← PoolManager 안의 실제 소환 로직 호출
			break;
		}
	}
}
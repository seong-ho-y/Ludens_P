// Copyright Epic Games, Inc. All Rights Reserved.


#include "Ludens_PPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnemyPoolManager.h"
#include "Ludens_PGameMode.h"
#include "EngineUtils.h"
#include "AI/NavigationSystemBase.h"
#include "Engine/LocalPlayer.h"
#include "EntitySystem/MovieSceneEntitySystemRunner.h"
#include "Kismet/GameplayStatics.h"

void ALudens_PPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}
void ALudens_PPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	InputComponent->BindAction("SpawnEnemy", IE_Pressed, this, &ALudens_PPlayerController::Server_RequestSpawnEnemy);
}


// 이 함수는 클라이언트의 요청을 받아 "서버에서" 실행될 실제 로직입니다.
void ALudens_PPlayerController::Server_RequestSpawnEnemy_Implementation()
{
	// 이제 이 코드는 100% 서버에서 실행됩니다.
    UE_LOG(LogTemp,Log,TEXT("Server_Request 호출 : PlayerController"));
	// 1. 서버 월드에 있는 PoolManager를 찾습니다.
	AEnemyPoolManager* PoolManager = Cast<AEnemyPoolManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyPoolManager::StaticClass()));
	if (PoolManager)
	{
		// 2. 서버의 가득 찬 창고에서 스폰을 호출합니다.
		FVector SpawnLoc = FVector(300.f, 300.f, 300.f);
		FRotator SpawnRot = FRotator::ZeroRotator;
        
		PoolManager->SpawnEnemy(WalkerEnemyBPClass, SpawnLoc, SpawnRot, EEnemyColor::Magenta);
		PoolManager->SpawnEnemy(TankerEnemyBPClass, SpawnLoc, SpawnRot, EEnemyColor::Cyan);
		//PoolManager->SpawnEnemy(RunnerEnemyBPClass, SpawnLoc, SpawnRot, EEnemyColor::Red);
		PoolManager->SpawnEnemy(SniperEnemyBPClass, SpawnLoc, SpawnRot, EEnemyColor::Green);
		PoolManager->SpawnEnemy(ExploEnemyBPClass, SpawnLoc, SpawnRot, EEnemyColor::Blue);
		PoolManager->SpawnEnemy(StealthEnemyBPClass, SpawnLoc, SpawnRot, EEnemyColor::Yellow);
		//PoolManager->SpawnEnemy(MagicEnemyBPClass, SpawnLoc, SpawnRot, EEnemyColor::Black);
	}
}

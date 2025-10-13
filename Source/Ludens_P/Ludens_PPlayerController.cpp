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
	UE_LOG(LogTemp, Log, TEXT("Server_Request 호출 : PlayerController"));

	// GameMode의 메서드 호출
	// GetAuthGameMode<T>()는 서버 월드의 GameMode를 T타입으로 안전하게 가져옵니다.
	ALudens_PGameMode* GM = GetWorld()->GetAuthGameMode<ALudens_PGameMode>();

	// GameMode를 성공적으로 가져왔는지 항상 확인합니다.
	if (GM)
	{
		// GM 스폰함수 호출
		GM->StartSpawningEnemies(); 
	}
}

void ALudens_PPlayerController::OnPossess(APawn* InPawn)
{
	// 부모 호출 필수
	Super::OnPossess(InPawn);

	// 로비에서 이미 커밋된 PlayerColor를 그대로 사용합니다.
	// 인게임에서 재배정(AssignColorToPlayer)하지 않습니다.
}
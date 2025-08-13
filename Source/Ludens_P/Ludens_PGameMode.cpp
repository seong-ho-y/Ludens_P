// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ludens_PGameMode.h"

#include "Ludens_PPlayerController.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

ALudens_PGameMode::ALudens_PGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	//여기서 BP 캐릭터를 디폴트 폰으로 설정
	//경로로 찾는 하드코딩을 하기에 경로 달라지면 오류 발생함 !!!!
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	if (PlayerPawnClassFinder.Class) //혹시나 BP의 이름이 바뀌거나 설치 경로가 바뀔 경우 nullptr 접근 오류 방지
	{
		DefaultPawnClass = PlayerPawnClassFinder.Class;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Failed to find BP Pawn!"));
	}

	//PlayerController 할당을 위해서 c++ 클래스를 할당해놓음
	PlayerControllerClass = ALudens_PPlayerController::StaticClass();

}
void ALudens_PGameMode::BeginPlay()
{
	Super::BeginPlay();
}


//멀티를 할 시 PlayerStart에서 Player가 스폰되는데 그 때 스폰되는 위치를 겹치지 않게 해주는 메서드
AActor* ALudens_PGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> AllStarts; //AllStart라는 액터 포인터 배열 - 여기에다가 PlayerStart위치를 받을거임

	//PlayerStart 클래스의 모든 객체를 AllStart 배열에 넣음
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), AllStarts);

	for (AActor* Start : AllStarts)
	{
		if (!UsedStartSpots.Contains(Start)) //같은 위치에서 스폰되지 않게 해주는 로직
		{
			UsedStartSpots.Add(Start);
			UE_LOG(LogTemp, Warning, TEXT("Player assigned to Start: %s"), *Start->GetName());
			return Start;
		}
	}

	// 모두 사용됐으면 기본 처리
	return Super::ChoosePlayerStart_Implementation(Player);
}
// 파일을 사용하는 C++ 파일 상단에 헤더를 포함합니다.
#include "EnemyPoolManager.h"
#include "Kismet/GameplayStatics.h"

// ...

void ALudens_PGameMode::SpawnOneEnemy()
{
	// 1. 레벨에 배치된 EnemyPoolManager를 찾습니다.
	// 이 코드는 한 번만 실행하고 결과를 저장해두는 것이 좋습니다 (예: BeginPlay)
	AEnemyPoolManager* PoolManager = Cast<AEnemyPoolManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyPoolManager::StaticClass()));

	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("레벨에 EnemyPoolManager가 없습니다!"));
		return;
	}

	// 2. 스폰할 적의 클래스와 위치를 정합니다.
	TSubclassOf<AEnemyBase> EnemyClassToSpawn = PoolManager->WalkerClass; // 매니저에 설정된 클래스를 가져오거나, 직접 지정
	FVector SpawnLocation = FVector(100.0f, 200.0f, 100.0f);
	FRotator SpawnRotation = FRotator::ZeroRotator;
    
	// 3. 관리자에게 적 스폰을 요청합니다.
	// 이 함수는 알아서 풀에 있는 적을 재활용하거나 새로 생성합니다.
	AEnemyBase* SpawnedEnemy = PoolManager->SpawnEnemy(EnemyClassToSpawn, SpawnLocation, SpawnRotation);
    
	// 이제 SpawnedEnemy를 가지고 필요한 로직을 수행할 수 있습니다.
	if (SpawnedEnemy)
	{
		UE_LOG(LogTemp, Log, TEXT("%s가 성공적으로 스폰되었습니다."), *SpawnedEnemy->GetName());
	}
}
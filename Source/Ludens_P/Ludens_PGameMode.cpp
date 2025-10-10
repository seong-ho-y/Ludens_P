// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ludens_PGameMode.h"
#include "EnemyPoolManager.h"
#include "Ludens_PPlayerController.h"
#include "PlayerStateComponent.h"
#include "RewardSystemComponent.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "EnemySpawnPoint.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"

class URewardSystemComponent;
class UPlayerStateComponent;

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

	ColorRotation.Add(EEnemyColor::Red);
	ColorRotation.Add(EEnemyColor::Green);
	ColorRotation.Add(EEnemyColor::Blue);
	//PlayerController 할당을 위해서 c++ 클래스를 할당해놓음
	PlayerControllerClass = ALudens_PPlayerController::StaticClass();

	bUseSeamlessTravel = true;
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
			//UE_LOG(LogTemp, Warning, TEXT("Player assigned to Start: %s"), *Start->GetName());
			return Start;
		}
	}

	// 모두 사용됐으면 기본 처리
	return Super::ChoosePlayerStart_Implementation(Player);
}


// GameMode의 OnPostLogin 함수 등에서 실행
void ALudens_PGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
}

void ALudens_PGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer); // Super 호출도 _Implementation으로 변경

	// 이제 이 안에서 AssignColorToPlayer를 호출하면 됩니다.
	AssignColorToPlayer(NewPlayer);
}

// ✨ 새로 추가된 함수의 전체 내용입니다.
void ALudens_PGameMode::AssignColorToPlayer(AController* NewPlayer)
{
	// --- 진단 로그 시작 ---
	//UE_LOG(LogTemp, Error, TEXT("--- AssignColorToPlayer CALLED for %s ---"), *NewPlayer->GetName());

	AGameStateBase* CurrentGameState = GetGameState<AGameStateBase>();
	if (!CurrentGameState)
	{
		UE_LOG(LogTemp, Error, TEXT("!!! CRITICAL ERROR: GameState is NULL. Cannot assign color. !!!"));
		return;
	}
    
	int32 PlayerCount = CurrentGameState->PlayerArray.Num();
	//UE_LOG(LogTemp, Warning, TEXT("Current Player Count from GameState->PlayerArray.Num(): %d"), PlayerCount);

	int32 PlayerIndex = PlayerCount - 1;
	//UE_LOG(LogTemp, Warning, TEXT("Calculated Player Index: %d"), PlayerIndex);
	// --- 진단 로그 끝 ---

	if (!ColorRotation.IsValidIndex(PlayerIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("!!! ERROR: Player Index %d is not valid for ColorRotation array!"), PlayerIndex);
		return;
	}

	APawn* PlayerPawn = NewPlayer->GetPawn();
	if (PlayerPawn)
	{
		UPlayerStateComponent* StateComp = PlayerPawn->FindComponentByClass<UPlayerStateComponent>();
		if (StateComp)
		{
			EEnemyColor NewColor = ColorRotation[PlayerIndex];
			StateComp->PlayerColor = NewColor;

			//UE_LOG(LogTemp, Warning, TEXT("SUCCESS: Assigned Color %s to Player with Index %d."), *UEnum::GetValueAsString(NewColor), PlayerIndex);
		}
	}
}
void ALudens_PGameMode::StartSpawningEnemies()
{
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("PoolManager를 찾을 수 없습니다!"));
		return;
	}

	// 1. 월드에 배치된 모든 스폰 포인트를 찾습니다.
	TArray<AActor*> SpawnPointActors;
	// 2. BP_EnemySpawnPoint 대신, 그것의 C++ 부모 클래스인 AEnemySpawnPoint를 찾습니다.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnPoint::StaticClass(), SpawnPointActors);

	if (SpawnPointActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("월드에 AEnemySpawnPoint 타입의 액터가 하나도 없습니다!"));
		return;
	}
    
	UE_LOG(LogTemp, Log, TEXT("%d개의 스폰 포인트를 찾았습니다. 스폰을 시작합니다."), SpawnPointActors.Num());

	// 2. 스폰할 적의 수를 정합니다. (예: 10마리)
	const int32 NumberOfEnemiesToSpawn = 10;
    
	// 스폰 가능한 포인트 수보다 많이 스폰하지 않도록 보정합니다.
	int32 ActualEnemiesToSpawn = FMath::Min(NumberOfEnemiesToSpawn, SpawnPointActors.Num());
    
	UE_LOG(LogTemp, Log, TEXT("%d개의 스폰 포인트 중 %d개의 적을 스폰합니다."), SpawnPointActors.Num(), ActualEnemiesToSpawn);

	for (int32 i = 0; i < ActualEnemiesToSpawn; ++i)
	{
		// 3. 사용 가능한 스폰 포인트 중에서 랜덤하게 하나를 고릅니다.
		int32 RandomIndex = FMath::RandRange(0, SpawnPointActors.Num() - 1);
		AActor* SelectedSpawnPoint = SpawnPointActors[RandomIndex];
		FVector SpawnLocation = SelectedSpawnPoint->GetActorLocation();

		// 4. (중요!) 한 번 사용한 스폰 포인트는 목록에서 제거하여 중복 스폰을 방지합니다.
		SpawnPointActors.RemoveAt(RandomIndex);

		// 5. CreateRandomEnemyProfile()을 호출하여 스폰할 적의 정보를 생성합니다.
		FEnemySpawnProfile ProfileToSpawn = CreateRandomEnemyProfile();
       
		// 6. 생성된 프로필과 위치를 사용하여 PoolManager에게 스폰을 요청합니다.
		PoolManager->SpawnEnemy(ProfileToSpawn, SpawnLocation, FRotator::ZeroRotator);
	}
}
FEnemySpawnProfile ALudens_PGameMode::CreateRandomEnemyProfile()
{
	FEnemySpawnProfile Profile;

	// 1. 적 타입 결정: 8개 중에 하나를 공평하게 뽑습니다.
	if (EnemyBPs.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, EnemyBPs.Num() - 1);
		Profile.EnemyClass = EnemyBPs[RandomIndex];
	}

	// 2. 색깔 결정: 확률에 따라 등급을 나눕니다.
	float ColorRoll = FMath::FRand(); // 0.0 ~ 1.0 사이의 랜덤 값
	if (ColorRoll < 0.5f) // 50% 확률: 약한 등급 (R, G, B)
	{
		Profile.Color = (EEnemyColor)FMath::RandRange(0, 2); // Red, Green, Blue 중 랜덤
	}
	else if (ColorRoll < 0.85f) // 35% 확률: 중간 등급 (Y, M, C)
	{
		Profile.Color = (EEnemyColor)FMath::RandRange(3, 5); // Yellow, Magenta, Cyan 중 랜덤
	}
	else // 15% 확률: 최강 등급 (Black)
	{
		Profile.Color = EEnemyColor::Black;
	}

	// 3. 강화형 결정: 25% 확률로 강화형이 됩니다.
	if (FMath::FRand() < 0.25f)
	{
		Profile.StatDataAsset = EnhancedStatDA;
	}
	else
	{
		Profile.StatDataAsset = nullptr;
	}

	return Profile;
}
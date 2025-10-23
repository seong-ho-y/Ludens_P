// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ludens_PGameMode.h"

#include "EnemyAIController.h"
#include "EnemyPoolManager.h"
#include "Ludens_PPlayerController.h"
#include "PlayerState_Real.h"
#include "Ludens_PCharacter.h"
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
	
	/*
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
	*/
	/// 아래와 같이 수정함.
	
	bUseSeamlessTravel = true;
	DefaultPawnClass = ALudens_PCharacter::StaticClass();
	//PlayerController 할당을 위해서 c++ 클래스를 할당해놓음
	PlayerControllerClass = ALudens_PPlayerController::StaticClass();
	PlayerStateClass = APlayerState_Real::StaticClass(); // 추가한 PlayerState


	ColorRotation.Add(EEnemyColor::Red);
	ColorRotation.Add(EEnemyColor::Green);
	ColorRotation.Add(EEnemyColor::Blue);

}
void ALudens_PGameMode::BeginPlay()
{
	Super::BeginPlay();
	PoolManager = Cast<AEnemyPoolManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyPoolManager::StaticClass()));
	if (!PoolManager)
	{
		UE_LOG(LogTemp, Error, TEXT("FATAL ERROR: PoolManager not found in the persistent level!"));
	}
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

// Ludens_PGameMode.cpp

void ALudens_PGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// 엔진의 PlayerState 할당은 SeamlessTravel 중에 일관성이 없을 수 있습니다.
	// 가장 확실한 데이터 전송 방법은 수동으로 확인하고 복사하는 것입니다.

	// ⭐ 1단계: 기존 레벨의 PlayerState를 가져옵니다.
	//    Super()가 호출되기 전에 NewPlayer에 임시로 할당될 수 있습니다.
	APlayerState* OldPS = NewPlayer ? NewPlayer->PlayerState : nullptr;

	// ⭐ 2단계: 부모 함수를 호출하여 엔진이 할당을 처리하도록 합니다.
	//    이 단계에서 새로운 PlayerState가 생성될 수 있습니다.
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	// ⭐ 3단계: 엔진이 새로 할당한 PlayerState를 가져옵니다.
	APlayerState_Real* NewPSR = NewPlayer ? NewPlayer->GetPlayerState<APlayerState_Real>() : nullptr;

	// ⭐ 4단계: 새로운 PlayerState가 생성되었다면, 기존 PlayerState의 데이터를 복사합니다.
	if (OldPS && NewPSR && NewPSR != OldPS)
	{
		UE_LOG(LogTemp, Warning, TEXT("Seamless travel failed. Manually copying data from old PlayerState to new one."));
		if (APlayerState_Real* OldRealPS = Cast<APlayerState_Real>(OldPS))
		{
			NewPSR->AppearanceId = OldRealPS->AppearanceId;
			NewPSR->SelectedColor = OldRealPS->SelectedColor;
			NewPSR->PlayerColor = OldRealPS->PlayerColor;
			NewPSR->SubskillId = OldRealPS->SubskillId;
			NewPSR->bReady = OldRealPS->bReady;

			UE_LOG(LogTemp, Display, TEXT("Data copied: Ap=%d, Color=%s"),
				NewPSR->AppearanceId, *UEnum::GetValueAsString(NewPSR->PlayerColor));
		}
	}

	// ⭐ 5단계: 최종 값을 로그로 출력하여 확인합니다.
	if (NewPSR)
	{
		UE_LOG(LogTemp, Display, TEXT("[InGameGM] FINAL PSR Values: Ap=%d Sel=%s Ply=%s Ready=%d  (Name=%s)"),
			NewPSR->AppearanceId,
			*UEnum::GetValueAsString(NewPSR->SelectedColor),
			*UEnum::GetValueAsString(NewPSR->PlayerColor),
			(int)NewPSR->bReady,
			*NewPSR->GetPlayerName());
	}

	AssignColorToPlayer(NewPlayer);
}


//  새로 추가된 함수의 전체 내용입니다.
void ALudens_PGameMode::AssignColorToPlayer(AController* NewPlayer)
{
	APlayerState_Real* PSR = NewPlayer ? NewPlayer->GetPlayerState<APlayerState_Real>() : nullptr;

	if (PSR)
	{
		// PSR이 존재하면, 로비에서 가져온 PlayerColor를 사용합니다.
		UE_LOG(LogTemp, Display, TEXT("[InGameGM] Final Color from Lobby: %s for player %s"),
			*UEnum::GetValueAsString(PSR->PlayerColor), *GetNameSafe(NewPlayer));
	}
	else
	{
		// PSR이 null일 때의 폴백 로직 (거의 실행되지 않아야 정상)
		UE_LOG(LogTemp, Warning, TEXT("[InGameGM] AssignColorToPlayer: PSR is NULL. Falling back to default color rotation."));

		AGameStateBase* CurrentGameState = GetGameState<AGameStateBase>();
		if (!CurrentGameState) return;

		int32 PlayerIndex = CurrentGameState->PlayerArray.Num() - 1;
		if (!ColorRotation.IsValidIndex(PlayerIndex)) return;

		EEnemyColor NewColor = ColorRotation[PlayerIndex];

		APawn* PlayerPawn = NewPlayer->GetPawn();
		if (PlayerPawn)
		{
			UPlayerStateComponent* StateComp = PlayerPawn->FindComponentByClass<UPlayerStateComponent>();
			if (StateComp)
			{
				StateComp->PlayerColor = NewColor;
				UE_LOG(LogTemp, Warning, TEXT("SUCCESS: Assigned default Color %s to Player with Index %d."), *UEnum::GetValueAsString(NewColor), PlayerIndex);
			}
		}
	}
}

bool ALudens_PGameMode::GetEnemyAllKilled() const
{
	if (!HasAuthority()) return false;
	return EnemyCount <= 0;
}

void ALudens_PGameMode::HandleEnemyDied(AEnemyBase* EnemyBase)
{
	if (HasAuthority())
	{
		EnemyCount--;
		//if (EnemyCount<=0) UE_LOG(LogTemp,Warning,TEXT("All Enemy Got Killed"));
	}
	
}

void ALudens_PGameMode::StartSpawningEnemies()
{
	if (!PoolManager)
    {
        UE_LOG(LogTemp, Error, TEXT("PoolManager를 찾을 수 없습니다!"));
        return;
    }

    // 1) 스폰 포인트 수집
    TArray<AActor*> SpawnPointActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawnPoint::StaticClass(), SpawnPointActors);

    const int32 SpawnPointCount = SpawnPointActors.Num();
    if (SpawnPointCount == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("월드에 AEnemySpawnPoint 타입의 액터가 하나도 없습니다!"));
        return;
    }

    //UE_LOG(LogTemp, Log, TEXT("%d개의 스폰 포인트를 찾았습니다. 스폰을 시작합니다."), SpawnPointCount);

    // 2) 스폰할 적 수 결정
    const int32 NumberOfEnemiesToSpawn = 20;
    const int32 ActualEnemiesToSpawn = FMath::Min(NumberOfEnemiesToSpawn, SpawnPointCount);

    //UE_LOG(LogTemp, Log, TEXT("%d개의 스폰 포인트 중 %d개의 적을 스폰합니다."), SpawnPointCount, ActualEnemiesToSpawn);

    // ✅ 실제 스폰 수로 EnemyCount 설정 (핵심 수정)
    EnemyCount = ActualEnemiesToSpawn;

    // 3) 스폰 루프
    for (int32 i = 0; i < ActualEnemiesToSpawn; ++i)
    {
        const int32 CurrentNum = SpawnPointActors.Num();
        if (CurrentNum <= 0)
        {
            UE_LOG(LogTemp, Error, TEXT("SpawnPointActors가 비었습니다. 루프 중단."));
            break;
        }

        // ✅ 배열 길이 기준(inclusive)으로 랜덤 인덱스 계산
        const int32 RandomIndex = FMath::RandRange(0, CurrentNum - 1);

        AActor* SelectedSpawnPoint = SpawnPointActors[RandomIndex];
        const FVector SpawnLocation = SelectedSpawnPoint->GetActorLocation();

        // 한 번 사용한 포인트 제거 → 다음 반복에서 Num()이 줄어듦
        SpawnPointActors.RemoveAt(RandomIndex);

        // 프로필 생성 및 스폰
        const FEnemySpawnProfile ProfileToSpawn = CreateRandomEnemyProfile();
        AEnemyBase* SpawnedEnemy = PoolManager->SpawnEnemy(ProfileToSpawn, SpawnLocation, FRotator::ZeroRotator);

        if (SpawnedEnemy)
        {
            // C++ 전용 델리게이트 바인딩
            SpawnedEnemy->OnEnemyDied.AddUObject(this, &ALudens_PGameMode::HandleEnemyDied);
        }
        else
        {
            // 스폰 실패 시 카운트 보정 (안전)
            --EnemyCount;
            UE_LOG(LogTemp, Error, TEXT("Enemy 스폰 실패: EnemyCount 보정 -> %d"), EnemyCount);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("최종 EnemyCount(살아있는 적 수): %d"), EnemyCount);
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
	if (ColorRoll < 0.3f) // 30% 확률: 약한 등급 (R, G, B)
	{
		Profile.Color = (EEnemyColor)FMath::RandRange(0, 2); // Red, Green, Blue 중 랜덤
	}
	else if (ColorRoll < 0.9f) // 60% 확률: 중간 등급 (Y, M, C)
	{
		Profile.Color = (EEnemyColor)FMath::RandRange(3, 5); // Yellow, Magenta, Cyan 중 랜덤
	}
	else // 10% 확률: 최강 등급 (Black)
	{
		Profile.Color = EEnemyColor::Black;
	}

	// 3. 강화형 결정: 5% 확률로 강화형이 됩니다.
	if (FMath::FRand() < 0.05f)
	{
		Profile.StatDataAsset = EnhancedStatDA;
	}
	else
	{
		Profile.StatDataAsset = nullptr;
	}

	return Profile;
}
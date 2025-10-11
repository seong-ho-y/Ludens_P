// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ludens_PGameMode.h"
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

	// (디버그)현재 GM / PC / PS / PAWN이 무엇인지 1회 찍어 확인
	UE_LOG(LogTemp, Log, TEXT("[InGameGM] GM=%s, PC=%s, Pawn=%s, PS=%s"),
		*GetClass()->GetName(),
		*GetNameSafe(PlayerControllerClass),
		*GetNameSafe(DefaultPawnClass),
		*GetNameSafe(PlayerStateClass));
	 

	// 이제 이 안에서 AssignColorToPlayer를 호출하면 됩니다.
	AssignColorToPlayer(NewPlayer);

}

//  새로 추가된 함수의 전체 내용입니다.
void ALudens_PGameMode::AssignColorToPlayer(AController* NewPlayer)
{

	if (APlayerState_Real* PSR = NewPlayer ? NewPlayer->GetPlayerState<APlayerState_Real>() : nullptr)
	{
		// 로비에서 Start 직전에 PSR->PlayerColor가 커밋되어 넘어옴 - 그대로 유지
		UE_LOG(LogTemp, Log, TEXT("[InGameGM] Keep lobby color: PlayerColor=%d Selected=%d Ready=%d"),
			(int32)PSR->PlayerColor, (int32)PSR->SelectedColor, PSR->bReady);
		return; //  덮어쓰기 금지
	}

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
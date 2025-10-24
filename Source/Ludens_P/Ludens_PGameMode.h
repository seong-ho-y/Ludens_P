// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "EEnemyColor.h"
#include "Ludens_PGameMode.generated.h"


class AEnemyBase;
class UEnemyDescriptor;
class AEnemyPoolManager;
enum class EEnemyColor : uint8;

USTRUCT(BlueprintType)
struct FEnemySpawnProfile
{
	GENERATED_BODY()

	// ... (EnemyClass, Color 변수는 그대로) ...

	// bool bIsEnhanced 대신 데이터 에셋 포인터를 사용합니다.
	// 이 포인터가 nullptr이면 '일반', 데이터 에셋이 할당되면 '강화형'입니다.
	UPROPERTY()
	TObjectPtr<UEnemyDescriptor> StatDataAsset;
	// 어떤 종류의 적을 스폰할지 (예: BP_Walker, BP_Shooter...)
	UPROPERTY()
	TSubclassOf<AEnemyBase> EnemyClass;

	// 어떤 색으로 스폰할지
	UPROPERTY()
	EEnemyColor Color;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllEnemiesKilled);

UCLASS(minimalapi)
class ALudens_PGameMode : public AGameMode
{
	GENERATED_BODY()

	//플레이어 시작 위치 받기
	UPROPERTY()
	TArray<AActor*> UsedStartSpots;
	UPROPERTY()
	AEnemyPoolManager* PoolManager;

	//적 생성
public:
	ALudens_PGameMode();
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override; //ChoosePlayerStart
	virtual void OnPostLogin(AController* NewPlayer) override;

	//Pawn이 준비된 후에 호출되는 함수
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	
	
	void BeginPlay();


	void AssignColorToPlayer(AController* NewPlayer);

	UFUNCTION(BlueprintCallable, Category = "Stage")
	bool GetEnemyAllKilled() const;
	void HandleEnemyDied(AEnemyBase* EnemyBase);
	
	void StartSpawningEnemies();

	
	// 랜덤 스폰 프로필을 생성하는 함수
	FEnemySpawnProfile CreateRandomEnemyProfile();
	int32 NumLoggedInPlayers = 0;
	TArray<EEnemyColor> ColorRotation;

	UPROPERTY(EditDefaultsOnly, Category="Spawning")
	TObjectPtr<UEnemyDescriptor> EnhancedStatDA;

	// 에디터에서 8종류의 적 BP를 지정할 배열
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TArray<TSubclassOf<AActor>> EnemyBPs;

	// 모두 처치 시 외부(룸)에서 바인딩 가능한 이벤트
	UPROPERTY(BlueprintAssignable, Category = "Stage")
	FOnAllEnemiesKilled OnAllEnemiesKilled;

	UFUNCTION(BlueprintCallable, Category = "Stage")
	int32 GetAliveEnemyCount() const { return EnemyCount; }

protected:
	int EnemyCount = 0;

public:
	// 한 명이라도 사망 시 서버에서 호출
	UFUNCTION() void NotifyAnyPlayerDead();

	// 클라이언트가 "재시작 준비됨" 버튼을 눌렀다고 서버에 알림
	UFUNCTION(Server, Reliable) void Server_ConfirmRestart(APlayerController* PC);

protected:
	// 게임오버 화면을 전원에게 띄움(클라 RPC)
	UFUNCTION(NetMulticast, Reliable) void Multicast_ShowGameOverScreen();

	// 게임오버 해제/재시작 카운트다운 시작
	void TryStartRestartCountdown();

	// 실제 맵 이동
	void DoServerTravel();

	// ====== 설정값 ======
	// 게임오버 후 이동할 맵(예: /Game/Maps/Lobby.Lobby)
	UPROPERTY(EditAnywhere, Category = "GameOver")
	TSoftObjectPtr<UWorld> RestartMap;

	// 필요한 플레이어 수(기본 3)
	UPROPERTY(EditAnywhere, Category = "GameOver")
	int32 RequiredPlayersForRestart = 3;

	// 카운트다운 초
	UPROPERTY(EditAnywhere, Category = "GameOver")
	float RestartDelaySeconds = 3.f;

	// ====== 상태 ======
	UPROPERTY(VisibleInstanceOnly)
	bool bGameOver = false;

	// 버튼을 누른 플레이어 컨트롤러의 ID 집합
	TSet<int32> ConfirmedControllerIds;

	FTimerHandle RestartTimer;
};


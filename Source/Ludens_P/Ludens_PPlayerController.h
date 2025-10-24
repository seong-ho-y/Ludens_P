// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "GameFramework/PlayerController.h"
#include "Ludens_PPlayerController.generated.h"

class UInputMappingContext;

/**
 *
 */
UCLASS()
class LUDENS_P_API ALudens_PPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;
	

	// Begin Actor interface

	virtual void BeginPlay() override;

public:
	virtual void SetupInputComponent() override;
	void OnPossess(APawn* InPawn);
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBase> WalkerEnemyBPClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBase> ShooterEnemyBPClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBase> TankerEnemyBPClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBase> RunnerEnemyBPClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBase> SniperEnemyBPClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBase> ExploEnemyBPClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBase> StealthEnemyBPClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBase> MagicEnemyBPClass;

	// "적 스폰을 요청"하는 서버 RPC 함수를 선언합니다.
	UFUNCTION(Server, Reliable)
	void Server_RequestSpawnEnemy();

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY() UUserWidget* GameOverWidget = nullptr;

	// 중복 클릭 방지
	UPROPERTY() bool bGameOverConfirmed = false;


	// 디버그: 키로 내 캐릭터 즉시 사망
	UFUNCTION(BlueprintCallable, Category = "Debug")
	void DebugKillMe();

	// 서버 권위로 사망 처리
	UFUNCTION(Server, Reliable)
	void Server_DebugKillMe();

	// 버튼이 누를 때 위젯에서 호출할 함수 (클라에서 호출됨)
	UFUNCTION(BlueprintCallable)
	void OnGameOverConfirmClicked();

	// 버튼 눌렀다고 서버에 알리는 RPC
	UFUNCTION(Server, Reliable)
	void Server_ConfirmRestart();

	// 게임오버 화면을 "각 클라에서" 띄우는 RPC
	UFUNCTION(Client, Reliable)
	void Client_ShowGameOverScreen();
};

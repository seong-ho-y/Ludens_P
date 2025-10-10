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
protected:
};

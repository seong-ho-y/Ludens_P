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
	void SpawnEnemyFromPool();
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBase> WalkerEnemyBPClass;
};

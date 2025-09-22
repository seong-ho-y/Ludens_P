// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RewardEffect.generated.h"

/**
 ���� ȿ���� ǥ���� �߻� Ŭ����
 */

struct FRewardRow;

UCLASS(Blueprintable)
class LUDENS_P_API URewardEffect : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Reward")
	void ApplyReward(ACharacter* Player, const FRewardRow& Row);
};

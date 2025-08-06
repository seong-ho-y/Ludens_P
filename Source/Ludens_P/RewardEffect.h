// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RewardEffect.generated.h"

/**
 보상 효과를 표현할 추상 클래스
 */

UCLASS(Blueprintable, Abstract)
class LUDENS_P_API URewardEffect : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Reward")
	void ApplyReward(ACharacter* Player);
};

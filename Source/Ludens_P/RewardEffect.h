// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RewardEffect.generated.h"

/**

 */

UCLASS(Blueprintable, Abstract)
class LUDENS_P_API URewardEffect : public UObject
{
	GENERATED_BODY()

public:
	// 1. 호출용 함수: 다른 곳에서 이 함수를 호출합니다. 여기에는 virtual을 쓰지 않습니다.
	UFUNCTION(BlueprintNativeEvent, Category = "Reward")
	void ApplyReward(ACharacter* TargetCharacter);

	// 2. C++ 구현용 함수: C++ 기본 로직을 여기에 작성하며, C++ 자식 클래스가 재정의(override)할 수 있도록 virtual을 붙입니다.
	virtual void ApplyReward_Implementation(ACharacter* TargetCharacter);

};
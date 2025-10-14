// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RewardEffect.generated.h"

/**
보상 적용 로직
*/

struct FRewardRow;

UCLASS(Abstract, Blueprintable)
class URewardEffect : public UObject
{
  GENERATED_BODY()

public:
  // 공통 진입점: 테이블 Row를 받아 적용
  UFUNCTION(BlueprintNativeEvent)
  void ApplyReward(ACharacter* Player, const FRewardRow& Row);
  virtual void ApplyReward_Implementation(ACharacter* Player, const FRewardRow& Row) {}
};

// 스탯형
UCLASS(Blueprintable)
class URewardEffect_Stat : public URewardEffect
{
  GENERATED_BODY()

public:
  virtual void ApplyReward_Implementation(ACharacter* Player, const FRewardRow& Row) override;

protected:
  void ApplyAdd(float& S, float D) { S += D; }
  void ApplyMul(float& S, float F) { S *= F; }
};

// 스킬형
UCLASS(Blueprintable)
class URewardEffect_Skill : public URewardEffect
{
  GENERATED_BODY()

public:
  virtual void ApplyReward_Implementation(ACharacter* Player, const FRewardRow& Row) override;
};
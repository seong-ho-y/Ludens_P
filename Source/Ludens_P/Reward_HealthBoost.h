// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RewardEffect.h"
#include "Reward_HealthBoost.generated.h"

/**
ü�� ���� ����
 */

UCLASS()
class LUDENS_P_API UReward_HealthBoost : public URewardEffect
{
    GENERATED_BODY()

public:
    virtual void ApplyReward_Implementation(ACharacter* Player) override;
};


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RewardEffect.h"
#include "RewardData.generated.h"

/**
 보상 데이터를 정의하는 구조체
 */

USTRUCT(BlueprintType)
struct FRewardData
{
	GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText RewardName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<URewardEffect> EffectClass;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RewardEffect.h"
#include "Engine/DataTable.h"
#include "RewardData.generated.h"

/**

 */

USTRUCT(BlueprintType)
struct FRewardData : public FTableRowBase //FTableRowBase를 상속받아서 쓰기
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName RewardName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<URewardEffect> EffectClass;
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RewardData.generated.h"

class URewardEffect;
class UTexture2D;

/**
 보상 데이터를 정의하는 구조체
 */

UENUM(BlueprintType)
enum class ERewardOpType : uint8
{
    Add,        // 합연산
    Multiply    // 곱연산
};

USTRUCT(BlueprintType)
struct FRewardRow : public FTableRowBase
{
    GENERATED_BODY()

    // 1) 이름
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText RewardName;

    // 2) 설명
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Description;

    // 3) 아이콘(소프트)
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UTexture2D> Icon;

    // 5) 증가할 수치 (예: 30 혹은 120)
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float Value = 0.f;

    // 6) 합/곱 연산 타입
    UPROPERTY(EditAnywhere, BlueprintReadOnly) ERewardOpType Op = ERewardOpType::Add;
    
    // 한 번만 허용할지 (예: 고유 유물, 중복 불가)
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bUniquePerRun = false;

    // 같은 그룹끼리는 중복 불가 (예: 'HP_Regeneration' 그룹 중 하나만)
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ExclusiveGroup;

    // 이 보상을 얻기 위해 필요한 태그(선행 보상/클래스 상태 등)
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FName> RequiresTags;

    // 이 보상과는 동시에 가질 수 없는 태그(상충/OP 조합 방지)
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FName> ExcludesTags;
};
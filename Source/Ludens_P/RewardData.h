// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RewardData.generated.h"

class URewardEffect;
class UTexture2D;

/**
보상을 관리할 DataTable
*/

UENUM(BlueprintType)
enum class ERewardOpType : uint8
{
    Add,        // 합연산
    Multiply    // 곱연산
};

UENUM(BlueprintType)
enum class ERewardType : uint8
{
    Stat,
    Skill
};

UENUM(BlueprintType)
enum class EPlayerReward : uint8
{
    MaxHP,
    MaxShield,
    MoveSpeed,
    ShieldRegenSpeed,
    DashRechargeTime,
    MaxDashCount,
    //JumpZVelocity,
    AttackDamage,
    WeaponAttackCoolTime,
    CriticalRate,
    CriticalDamage,
    AbsorbDelay,
    MaxSavedAmmo,
    MaxAmmo
};

USTRUCT(BlueprintType)
struct FRewardRow : public FTableRowBase
{
    GENERATED_BODY()

    // 1) 이름
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText RewardName;

    // 2) 설명
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Description;

    // 3) 아이콘
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UTexture2D> Icon;

    // 4) 증가 수치
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float Value = 0.f;

    // 5) 명시되어 있는 보상 종류
    UPROPERTY(EditAnywhere, BlueprintReadOnly) ERewardType TargetRewardType = ERewardType::Skill;
    UPROPERTY(EditAnywhere, BlueprintReadOnly) EPlayerReward TargetReward;
    // 6) 합/곱
    UPROPERTY(EditAnywhere, BlueprintReadOnly) ERewardOpType Op = ERewardOpType::Add;
    
    /**
    // 한 게임 동안 한 번만 허용할지
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bUniquePerRun = false;

    // 같은 그룹끼리는 중복 불가
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ExclusiveGroup;

    // 보상을 얻기 위한 선행 태그
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FName> RequiresTags;

    // 보상과 동시에 얻을 수 없는 태그
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FName> ExcludesTags;
    */
};
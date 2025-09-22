// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RewardData.generated.h"

class URewardEffect;
class UTexture2D;

/**
 ���� �����͸� �����ϴ� ����ü
 */

UENUM(BlueprintType)
enum class ERewardOpType : uint8
{
    Add,        // �տ���
    Multiply    // ������
};

USTRUCT(BlueprintType)
struct FRewardRow : public FTableRowBase
{
    GENERATED_BODY()

    // 1) �̸�
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText RewardName;

    // 2) ����
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FText Description;

    // 3) ������(����Ʈ)
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TSoftObjectPtr<UTexture2D> Icon;

    // 5) ������ ��ġ (��: 30 Ȥ�� 120)
    UPROPERTY(EditAnywhere, BlueprintReadOnly) float Value = 0.f;

    // 6) ��/�� ���� Ÿ��
    UPROPERTY(EditAnywhere, BlueprintReadOnly) ERewardOpType Op = ERewardOpType::Add;
    
    // �� ���� ������� (��: ���� ����, �ߺ� �Ұ�)
    UPROPERTY(EditAnywhere, BlueprintReadOnly) bool bUniquePerRun = false;

    // ���� �׷쳢���� �ߺ� �Ұ� (��: 'HP_Regeneration' �׷� �� �ϳ���)
    UPROPERTY(EditAnywhere, BlueprintReadOnly) FName ExclusiveGroup;

    // �� ������ ��� ���� �ʿ��� �±�(���� ����/Ŭ���� ���� ��)
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FName> RequiresTags;

    // �� ������� ���ÿ� ���� �� ���� �±�(����/OP ���� ����)
    UPROPERTY(EditAnywhere, BlueprintReadOnly) TArray<FName> ExcludesTags;
};
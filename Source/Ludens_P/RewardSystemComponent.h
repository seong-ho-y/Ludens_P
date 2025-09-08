// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RewardData.h"
#include "RewardSystemComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUDENS_P_API URewardSystemComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	URewardSystemComponent();

    /** ���� ��ü Ǯ ��� (BP���� ���) */
    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    TArray<FRewardData> AllRewards;

    /** UI Ŭ���� (���� ����) */
    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    TSubclassOf<class URewardUIWidget> RewardUIClass;

    /** ������ ���Կ� �����ϴ� 'AllRewards' �ε�����(���� ����) */
    UPROPERTY()
    TArray<int32> LastOfferedIndices;

public:
    /** ���� ���� �Լ� */
    void ApplyReward(const FRewardData& Data);

public:
    /* ������ ���� 3���� �̾� �ش� �÷��̾� Ŭ�󿡸� UI ���� */
    UFUNCTION(Server, Reliable)
    void Server_ShowRewardOptions();

    /* �������ش� �÷��̾�Ը� �����ϴ� Client RPC(�ε����� ����) */
    UFUNCTION(Client, Reliable)
    void Client_ShowRewardUI(const TArray<int32>& OptionIndices);

    /* (���� ����) �÷��̾ �� ���� �ε���(0~2)�� ������ */
    UFUNCTION(Server, Reliable)
    void Server_SelectReward(int32 ChoiceSlotIndex);

    UFUNCTION(Client, Reliable)
    void Client_EnableInputAfterReward();

private:
    URewardUIWidget* ActiveRewardWidget;  // ���� ����� ���� ���� ����
};

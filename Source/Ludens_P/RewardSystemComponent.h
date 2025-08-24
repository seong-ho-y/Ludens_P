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

	/** ���� ����� �������� ������ */
    UFUNCTION(BlueprintCallable)
    void ShowRewardOptions();

    /** ���� ���� */
    UFUNCTION(Server, Reliable)
    void Server_SelectReward(int32 Index);

protected:
    /** ���� ��ü Ǯ ��� (BP���� ���) */
    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    TArray<FRewardData> AllRewards;

    /** ���� ���õ� 3���� ���� �ĺ� */
    TArray<FRewardData> CurrentChoices;

    /** UI Ŭ���� (���� ����) */
    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    TSubclassOf<class URewardUIWidget> RewardUIClass;

    /** ���� ���� �Լ� */
    void ApplyReward(const FRewardData& Data);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_ShowRewardUI(const TArray<FRewardData>& InChoices); // UI ���� ����

    UFUNCTION(Client, Reliable)
    void Client_EnableInputAfterReward();

private:
    URewardUIWidget* ActiveRewardWidget;  // ���� ����� ���� ���� ����
};

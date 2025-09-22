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

    /** ���� ��ü ��� */
    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    TSoftObjectPtr<UDataTable> RewardTable;

    UPROPERTY()
    TArray<FName> LastOfferedRowNames;

    /** UI Ŭ���� (���� ����) */
    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    TSubclassOf<class URewardUIWidget> RewardUIClass;

public:
    /** ���� ���� �Լ� */
    void ApplyReward(const FRewardRow& Row);

    bool GetRowData(FName RowName, FRewardRow& Out) const;

public:
    /* ������ ���� 3���� �̾� �ش� �÷��̾� Ŭ�󿡸� UI ���� */
    UFUNCTION(Server, Reliable)
    void Server_ShowRewardOptions();

    /* �������ش� �÷��̾�Ը� �����ϴ� Client RPC(�ε����� ����) */
    UFUNCTION(Client, Reliable)
    void Client_ShowRewardUI(const TArray<FName>& OptionRowNames);

    /* (���� ����) �÷��̾ ���� ���� �ε���(0~2)�� ������ */
    UFUNCTION(Server, Reliable)
    void Server_SelectReward(FName PickedRowName);

    UFUNCTION(Client, Reliable)
    void Client_EnableInputAfterReward();

private:
    URewardUIWidget* ActiveRewardWidget = nullptr;  // ���� ����� ���� ���� ����
};

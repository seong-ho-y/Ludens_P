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

    /** 보상 전체 풀 목록 (BP에서 등록) */
    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    TArray<FRewardData> AllRewards;

    /** UI 클래스 (보상 위젯) */
    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    TSubclassOf<class URewardUIWidget> RewardUIClass;

    /** 보여줄 슬롯에 대응하는 'AllRewards' 인덱스들(서버 전용) */
    UPROPERTY()
    TArray<int32> LastOfferedIndices;

public:
    /** 보상 적용 함수 */
    void ApplyReward(const FRewardData& Data);

public:
    /* 서버가 보상 3개를 뽑아 해당 플레이어 클라에만 UI 전송 */
    UFUNCTION(Server, Reliable)
    void Server_ShowRewardOptions();

    /* 서버→해당 플레이어에게만 전송하는 Client RPC(인덱스만 전송) */
    UFUNCTION(Client, Reliable)
    void Client_ShowRewardUI(const TArray<int32>& OptionIndices);

    /* (서버 권위) 플레이어가 고른 슬롯 인덱스(0~2)를 서버로 */
    UFUNCTION(Server, Reliable)
    void Server_SelectReward(int32 ChoiceSlotIndex);

    UFUNCTION(Client, Reliable)
    void Client_EnableInputAfterReward();

private:
    URewardUIWidget* ActiveRewardWidget;  // 현재 띄워진 위젯 참조 저장
};

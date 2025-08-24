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

	/** 보상 목록을 무작위로 보여줌 */
    UFUNCTION(BlueprintCallable)
    void ShowRewardOptions();

    /** 보상 선택 */
    UFUNCTION(Server, Reliable)
    void Server_SelectReward(int32 Index);

protected:
    /** 보상 전체 풀 목록 (BP에서 등록) */
    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    TArray<FRewardData> AllRewards;

    /** 현재 선택된 3가지 보상 후보 */
    TArray<FRewardData> CurrentChoices;

    /** UI 클래스 (보상 위젯) */
    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    TSubclassOf<class URewardUIWidget> RewardUIClass;

    /** 보상 적용 함수 */
    void ApplyReward(const FRewardData& Data);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_ShowRewardUI(const TArray<FRewardData>& InChoices); // UI 생성 전용

    UFUNCTION(Client, Reliable)
    void Client_EnableInputAfterReward();

private:
    URewardUIWidget* ActiveRewardWidget;  // 현재 띄워진 위젯 참조 저장
};

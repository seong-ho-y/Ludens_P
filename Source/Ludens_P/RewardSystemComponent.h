// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RewardData.h"
#include "Engine/DataTable.h"
#include "RewardSystemComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LUDENS_P_API URewardSystemComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	URewardSystemComponent();
	void GenerateAndShowRewardsForOwner();

	UPROPERTY(EditAnywhere, Category = "Reward System")
	UDataTable* RewardDataTable;

	UFUNCTION(BlueprintCallable)
	void ShowRewardOptions();


	UFUNCTION(Server, Reliable)
	void Server_SelectReward(int32 Index);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	TArray<FRewardData> AllRewards;

	UFUNCTION(Client, Reliable)
	void Client_ShowRewardUI(const TArray<FRewardData>& RewardChoices);
	
	TArray<FRewardData> CurrentChoices;
	
	UPROPERTY(EditDefaultsOnly, Category = "Reward")
	TSubclassOf<class URewardUIWidget> RewardUIClass;
	
	void ApplyReward(const FRewardData& Data);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ShowRewardUI();

	UFUNCTION(Client, Reliable)
	void Client_EnableInputAfterReward();

private:
	URewardUIWidget* ActiveRewardWidget;
};
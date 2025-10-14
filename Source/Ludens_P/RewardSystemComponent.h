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

    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    TSoftObjectPtr<UDataTable> RewardTable;

    UPROPERTY()
    TArray<FName> LastOfferedRowNames;

    UPROPERTY(EditDefaultsOnly, Category = "Reward")
    TSubclassOf<class URewardUIWidget> RewardUIClass;

public:
    void ApplyReward(const FRewardRow& Row);
    bool GetRowData(FName RowName, FRewardRow& Out) const;

public:
    UFUNCTION(Server, Reliable)
    void Server_ShowRewardOptions();

    UFUNCTION(Client, Reliable)
    void Client_ShowRewardUI(const TArray<FName>& OptionRowNames);

    UFUNCTION(Server, Reliable)
    void Server_SelectReward(FName PickedRowName);

    UFUNCTION(Client, Reliable)
    void Client_EnableInputAfterReward();

private:
    URewardUIWidget* ActiveRewardWidget = nullptr;
};

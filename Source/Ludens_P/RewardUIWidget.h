// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RewardData.h"
#include "RewardUIWidget.generated.h"

/**
 *
 */

class UButton;
class ACharacter;

UCLASS()
class LUDENS_P_API URewardUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Reward")
	void SetRewardList(const TArray<FRewardData>& Rewards);
	
	void SetOwnerPlayer(ACharacter* OwnerChar);

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* Button_Reward0;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Reward1;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_Reward2;

private:
	UFUNCTION()
	void OnReward0Clicked();

	UFUNCTION()
	void OnReward1Clicked();

	UFUNCTION()
	void OnReward2Clicked();
	
	ACharacter* OwningPlayer;
	
	void SelectRewardByIndex(int32 Index);
public:
	UFUNCTION(BlueprintCallable)
	void RemoveSelf();  // �ܺο��� ���� ȣ��
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RewardData.h"
#include "RewardUIWidget.generated.h"

/**
 보상 UI 위젯
 */

class UButton;
class ACharacter;

UCLASS()
class LUDENS_P_API URewardUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

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

public:
    UFUNCTION(BlueprintCallable)
    void RemoveSelf();  // 외부에서 제거 호출
};
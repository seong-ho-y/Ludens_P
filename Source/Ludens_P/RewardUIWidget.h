// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RewardData.h"
#include "RewardUIWidget.generated.h"

/**
 ���� UI ����
 */

class UButton;
class UTextBlock;
class UImage;
class ACharacter;

USTRUCT(BlueprintType)
struct FRewardUIData
{
    GENERATED_BODY()
    UPROPERTY(BlueprintReadOnly) FName RowName;
    UPROPERTY(BlueprintReadOnly) FText Title;
    UPROPERTY(BlueprintReadOnly) FText Body;
    UPROPERTY(BlueprintReadOnly) UTexture2D* Icon = nullptr;
};

UCLASS()
class LUDENS_P_API URewardUIWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    // 첫번째 보상
    UPROPERTY(meta = (BindWidget)) UButton* Button_Reward0;
    UPROPERTY(meta = (BindWidget)) UImage* Image_Icon0;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Title0;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Body0;

    // 두번째 보상
    UPROPERTY(meta = (BindWidget)) UButton* Button_Reward1;
    UPROPERTY(meta = (BindWidget)) UImage* Image_Icon1;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Title1;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Body1;

    // 세 번째 보상
    UPROPERTY(meta = (BindWidget)) UButton* Button_Reward2;
    UPROPERTY(meta = (BindWidget)) UImage* Image_Icon2;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Title2;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Body2;

public:
    virtual void NativeConstruct() override;
    void InitWithRows(ACharacter* OwnerChar, const TArray<FRewardUIData>& InList);

private:
    void FillSlot(int32 Index, UImage* IconW, UTextBlock* TitleW, UTextBlock* BodyW);

public:
    UFUNCTION(BlueprintCallable) void RemoveSelf();

private:
    UFUNCTION() void OnReward0Clicked();
    UFUNCTION() void OnReward1Clicked();
    UFUNCTION() void OnReward2Clicked();

private:
    ACharacter* OwningPlayer = nullptr;
    TArray<FRewardUIData> Slots;
};
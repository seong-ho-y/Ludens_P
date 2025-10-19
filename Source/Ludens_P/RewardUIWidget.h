// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RewardData.h"
#include "Containers/Ticker.h"
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
    virtual void NativeDestruct() override;

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

    // ----- Intro animation params -----
    UPROPERTY(EditAnywhere, Category = "Intro")
    float IntroStagger = 0.24f;          // 슬롯 간 지연
    UPROPERTY(EditAnywhere, Category = "Intro")
    float IntroDuration = 0.5f;         // 1개 슬롯 펼침 시간

    struct FSlotIntro
    {
        bool  bScheduled = false;
        bool  bPlaying = false;
        bool  bCompleted = false;   
        float StartAt = 0.f;   // world time
        float Elapsed = 0.f;
        UWidget* Target = nullptr;
    };
    FSlotIntro SlotAnims[3];

    bool bAnyPlaying = false;

    void SetupInitialIntroState(UWidget* Target);
    void StartSlot(int32 Index, UWidget* Target, float StartAt);
    void TickSlot(FSlotIntro& S, float DT);
    float EaseOutBack(float T) const; // “양피지 펼침” 느낌의 이징

    // Ticker 사용
    FTSTicker::FDelegateHandle IntroTickerHandle;
    bool bIntroTickerActive = false;

    // Ticker 콜백 (매 프레임 호출)
    bool HandleIntroTick(float DeltaTime);  // ⬅️ 반환값: 계속 true, 정지 false

};
// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardUIWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "RewardSystemComponent.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"

void URewardUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_Reward0) Button_Reward0->OnClicked.AddDynamic(this, &URewardUIWidget::OnReward0Clicked);
    if (Button_Reward1) Button_Reward1->OnClicked.AddDynamic(this, &URewardUIWidget::OnReward1Clicked);
    if (Button_Reward2) Button_Reward2->OnClicked.AddDynamic(this, &URewardUIWidget::OnReward2Clicked);
}

void URewardUIWidget::SetRewardList(const TArray<FRewardData>& Rewards)
{
    CurrentRewards = Rewards; // 선택 검증이나 리렌더시 사용 가능

    if (Rewards.IsValidIndex(0)) FillSlot(0, Image_Icon0, Text_Title0, Text_Body0);
    if (Rewards.IsValidIndex(1)) FillSlot(1, Image_Icon1, Text_Title1, Text_Body1);
    if (Rewards.IsValidIndex(2)) FillSlot(2, Image_Icon2, Text_Title2, Text_Body2);
}

void URewardUIWidget::SetOwnerPlayer(ACharacter* OwnerChar)
{
    OwningPlayer = OwnerChar;
}

void URewardUIWidget::OnReward0Clicked()
{
    if (!OwningPlayer) return;

    if (URewardSystemComponent* Comp = OwningPlayer->FindComponentByClass<URewardSystemComponent>())
    {
        Comp->Server_SelectReward(0);
    }

    if (GEngine)
    {
        FString Msg = FString::Printf(TEXT("Selet Reward 0"));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Msg);
    }
}

void URewardUIWidget::OnReward1Clicked()
{
    if (!OwningPlayer) return;

    if (URewardSystemComponent* Comp = OwningPlayer->FindComponentByClass<URewardSystemComponent>())
    {
        Comp->Server_SelectReward(1);
    }

    if (GEngine)
    {
        FString Msg = FString::Printf(TEXT("Selet Reward 1"));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Msg);
    }
}

void URewardUIWidget::OnReward2Clicked()
{
    if (!OwningPlayer) return;

    if (URewardSystemComponent* Comp = OwningPlayer->FindComponentByClass<URewardSystemComponent>())
    {
        Comp->Server_SelectReward(2);
    }

    if (GEngine)
    {
        FString Msg = FString::Printf(TEXT("Selet Reward 2"));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Msg);
    }
}

void URewardUIWidget::FillSlot(int32 Index, UImage* IconW, UTextBlock* TitleW, UTextBlock* BodyW)
{
    const FRewardData& Data = CurrentRewards[Index];

    if (TitleW) TitleW->SetText(Data.RewardName); // FText 그대로 사용
    if (BodyW)  BodyW->SetText(FText::FromString(Data.Description));
    if (IconW)
    {
        if (Data.Icon) { IconW->SetBrushFromTexture(Data.Icon); IconW->SetVisibility(ESlateVisibility::Visible); }
        else { IconW->SetVisibility(ESlateVisibility::Collapsed); }
    }
}

void URewardUIWidget::RemoveSelf()
{
    RemoveFromParent();
}
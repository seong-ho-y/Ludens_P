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

void URewardUIWidget::InitWithRows(ACharacter* OwnerChar, const TArray<FRewardUIData>& InList)
{
    OwningPlayer = OwnerChar;
    Slots = InList;
    if (Slots.IsValidIndex(0)) FillSlot(0, Image_Icon0, Text_Title0, Text_Body0);
    if (Slots.IsValidIndex(1)) FillSlot(1, Image_Icon1, Text_Title1, Text_Body1);
    if (Slots.IsValidIndex(2)) FillSlot(2, Image_Icon2, Text_Title2, Text_Body2);
}

void URewardUIWidget::OnReward0Clicked()
{
    if (!OwningPlayer || !Slots.IsValidIndex(0)) return;

    if (URewardSystemComponent* Comp = OwningPlayer->FindComponentByClass<URewardSystemComponent>())
    {
        Comp->Server_SelectReward(Slots[0].RowName);
    }
}

void URewardUIWidget::OnReward1Clicked()
{
    if (!OwningPlayer || !Slots.IsValidIndex(1)) return;

    if (URewardSystemComponent* Comp = OwningPlayer->FindComponentByClass<URewardSystemComponent>())
    {
        Comp->Server_SelectReward(Slots[1].RowName);
    }
}

void URewardUIWidget::OnReward2Clicked()
{
    if (!OwningPlayer || !Slots.IsValidIndex(2)) return;

    if (URewardSystemComponent* Comp = OwningPlayer->FindComponentByClass<URewardSystemComponent>())
    {
        Comp->Server_SelectReward(Slots[2].RowName);
    }
}

void URewardUIWidget::FillSlot(int32 Index, UImage* IconW, UTextBlock* TitleW, UTextBlock* BodyW)
{
    const FRewardUIData& Data = Slots[Index];

    if (TitleW) TitleW->SetText(Data.Title);
    if (BodyW)  BodyW->SetText(Data.Body);
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
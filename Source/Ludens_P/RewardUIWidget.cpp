// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardUIWidget.h"
#include "Components/Button.h"
#include "RewardSystemComponent.h"
#include "GameFramework/Character.h"

void URewardUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (Button_Reward0) Button_Reward0->OnClicked.AddDynamic(this, &URewardUIWidget::OnReward0Clicked);
    if (Button_Reward1) Button_Reward1->OnClicked.AddDynamic(this, &URewardUIWidget::OnReward1Clicked);
    if (Button_Reward2) Button_Reward2->OnClicked.AddDynamic(this, &URewardUIWidget::OnReward2Clicked);
}

void URewardUIWidget::SetRewardList(const TArray<FRewardData>& Rewards)
{
    // (아직 아무것도 안 해도 됨)
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

void URewardUIWidget::RemoveSelf()
{
    RemoveFromParent();
}
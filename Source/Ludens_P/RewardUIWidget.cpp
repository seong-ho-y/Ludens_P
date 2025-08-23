// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardUIWidget.h"
#include "Components/Button.h"
#include "RewardSystemComponent.h"
#include "GameFramework/Character.h"


void URewardUIWidget::SetOwnerPlayer(ACharacter* OwnerChar)
{
    OwningPlayer = OwnerChar;
}

void URewardUIWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 각 버튼을 전용 UFUNCTION에 다시 연결합니다.
    if (Button_Reward0) Button_Reward0->OnClicked.AddDynamic(this, &URewardUIWidget::OnReward0Clicked);
    if (Button_Reward1) Button_Reward1->OnClicked.AddDynamic(this, &URewardUIWidget::OnReward1Clicked);
    if (Button_Reward2) Button_Reward2->OnClicked.AddDynamic(this, &URewardUIWidget::OnReward2Clicked);
}
void URewardUIWidget::OnReward0Clicked()
{
    SelectRewardByIndex(0);
}

void URewardUIWidget::OnReward1Clicked()
{
    SelectRewardByIndex(1);
}

void URewardUIWidget::OnReward2Clicked()
{
    SelectRewardByIndex(2);
}

// 통합된 함수
void URewardUIWidget::SelectRewardByIndex(int32 Index)
{
    if (!OwningPlayer) return;

    if (URewardSystemComponent* Comp = OwningPlayer->FindComponentByClass<URewardSystemComponent>())
    {
        Comp->Server_SelectReward(Index);
    }

    if (GEngine)
    {
        FString Msg = FString::Printf(TEXT("Selected Reward %d"), Index);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Msg);
    }
}
void URewardUIWidget::RemoveSelf()
{
    RemoveFromParent();
}
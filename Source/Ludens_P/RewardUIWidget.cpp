// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardUIWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Widgets/SWidget.h"
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

void URewardUIWidget::NativeDestruct()
{
    if (bIntroTickerActive)
    {
        FTSTicker::GetCoreTicker().RemoveTicker(IntroTickerHandle);
        bIntroTickerActive = false;
    }
    Super::NativeDestruct();
}

void URewardUIWidget::InitWithRows(ACharacter* OwnerChar, const TArray<FRewardUIData>& InList)
{
    OwningPlayer = OwnerChar;
    Slots = InList;
    if (Slots.IsValidIndex(0)) FillSlot(0, Image_Icon0, Text_Title0, Text_Body0);
    if (Slots.IsValidIndex(1)) FillSlot(1, Image_Icon1, Text_Title1, Text_Body1);
    if (Slots.IsValidIndex(2)) FillSlot(2, Image_Icon2, Text_Title2, Text_Body2);

    // --- 인트로 초기 상태(접힌 양피지)로 세팅
    if (Button_Reward0) SetupInitialIntroState(Button_Reward0);
    if (Button_Reward1) SetupInitialIntroState(Button_Reward1);
    if (Button_Reward2) SetupInitialIntroState(Button_Reward2);

    // --- 슬롯별 순차 재생 스케줄
    const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
    int32 idx = 0;
    if (Button_Reward0) StartSlot(idx++, Button_Reward0, Now + IntroStagger * 0);
    if (Button_Reward1) StartSlot(idx++, Button_Reward1, Now + IntroStagger * 1);
    if (Button_Reward2) StartSlot(idx++, Button_Reward2, Now + IntroStagger * 2);

    bAnyPlaying = true;

    if (Button_Reward0) Button_Reward0->SetIsEnabled(false);
    if (Button_Reward1) Button_Reward1->SetIsEnabled(false);
    if (Button_Reward2) Button_Reward2->SetIsEnabled(false);

    // FTSTicker로 프레임 콜백 등록
    if (!bIntroTickerActive)
    {
        IntroTickerHandle = FTSTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateUObject(this, &URewardUIWidget::HandleIntroTick)
        );
        bIntroTickerActive = true;
    }
}

bool URewardUIWidget::HandleIntroTick(float DeltaTime)
{
    if (!bAnyPlaying)
    {
        bIntroTickerActive = false;
        return false; // ticker 정지
    }

    int32 PlayingCount = 0;
    for (FSlotIntro& S : SlotAnims)
    {
        // 이미 끝난 슬롯은 완전히 스킵
        if (S.bCompleted) continue;

        if (!S.bScheduled) continue;

        const float Now = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
        if (!S.bPlaying)
        {
            if (Now >= S.StartAt) { S.bPlaying = true; S.Elapsed = 0.f; }
            else { continue; }
        }

        TickSlot(S, DeltaTime);
        if (S.bPlaying) ++PlayingCount;
    }

    if (PlayingCount == 0)
    {
        bAnyPlaying = false;
        bIntroTickerActive = false;

        if (Button_Reward0) Button_Reward0->SetIsEnabled(true);
        if (Button_Reward1) Button_Reward1->SetIsEnabled(true);
        if (Button_Reward2) Button_Reward2->SetIsEnabled(true);

        return false; // ticker 정지
    }

    return true; // 계속 호출
}


void URewardUIWidget::SetupInitialIntroState(UWidget* Target)
{
    if (!Target) return;
    
    // 중앙에서 위/아래로 동시에 펼쳐지도록 Pivot 중앙
    Target->SetRenderTransformPivot(FVector2D(0.5f, 0.5f));

    FWidgetTransform T;
    T.Translation = FVector2D(0.f, 0.f);
    T.Scale = FVector2D(1.f, 0.05f);
    T.Shear = FVector2D::ZeroVector;
    T.Angle = 0.f;
    Target->SetRenderTransform(T);

    Target->SetRenderOpacity(0.f);
}

void URewardUIWidget::StartSlot(int32 Index, UWidget* Target, float StartAt)
{
    if (Index < 0 || Index >= 3) return;
    SlotAnims[Index].bScheduled = true;
    SlotAnims[Index].bPlaying = false; // StartAt 도달 시 시작
    SlotAnims[Index].StartAt = StartAt;
    SlotAnims[Index].Elapsed = 0.f;
    SlotAnims[Index].Target = Target;
}

void URewardUIWidget::TickSlot(FSlotIntro& S, float DT)
{
    if (!S.Target || !S.bPlaying) return;

    S.Elapsed += DT;
    float t = FMath::Clamp(S.Elapsed / FMath::Max(0.01f, IntroDuration), 0.f, 1.f);

    // “펼쳐지는” 느낌의 EaseOutBack (t>1에서 살짝 초과 후 안정)
    const float e = EaseOutBack(t);

    // 1) 페이드인
    S.Target->SetRenderOpacity(e);

    // 2) 중앙 펼침: Translation 고정
    const float y = 0.f;

    // 3) 세로 스케일 업 (0.05 → 1.0) : 양피지 펼침
    const float scaleY = FMath::Lerp(0.05f, 1.f, e);

    FWidgetTransform T = S.Target->GetRenderTransform();
    T.Translation = FVector2D(0.f, y);
    T.Scale = FVector2D(1.f, scaleY);
    S.Target->SetRenderTransform(T);

    if (t >= 1.f)
    {
        // 마무리값 고정
        S.Target->SetRenderOpacity(1.f);
        T.Translation = FVector2D(0.f, 0.f);
        T.Scale = FVector2D(1.f, 1.f);
        S.Target->SetRenderTransform(T);

        S.bPlaying = false;
        S.bScheduled = false;
        S.bCompleted = true;
    }
}

float URewardUIWidget::EaseOutBack(float T) const
{
    // 표준 easeOutBack 계열
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.f;
    return 1.f + c3 * FMath::Pow(T - 1.f, 3) + c1 * FMath::Pow(T - 1.f, 2);
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
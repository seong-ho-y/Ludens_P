// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInfoWidget.h"

void UGameInfoWidget::NativeDestruct()
{
    if (bTickerActive)
    {
        FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
        bTickerActive = false;
    }
    Super::NativeDestruct();
}

void UGameInfoWidget::Apply(float Scale, float Opacity, float OffsetY)
{
    SetRenderTransformPivot(FVector2D(0.5f, 0.5f));

    FWidgetTransform T = GetRenderTransform();
    T.Scale = FVector2D(Scale, Scale);
    T.Translation = FVector2D(0.f, OffsetY);
    SetRenderTransform(T);

    if (bFadeOpacity)
    {
        SetRenderOpacity(Opacity);
    }
}

void UGameInfoWidget::BeginAnim(EMode NewMode, float DurationSec)
{
    Mode = NewMode;
    Duration = FMath::Max(0.01f, DurationSec);
    Elapsed = 0.f;

    if (Mode == EMode::Appearing)
    {
        const float startOp = bFadeOpacity ? 0.f : 1.f;
        const float startY = bUseSubtleMove ? -8.f : 0.f;
        Apply(StartScale, startOp, startY);
        // 입력 비차단: 히트테스트 불가 상태로 표시
        SetVisibility(ESlateVisibility::HitTestInvisible);
    }
    else // Disappearing
    {
        Apply(1.f, bFadeOpacity ? 1.f : 1.f, 0.f);
        SetVisibility(ESlateVisibility::HitTestInvisible);
    }

    if (!bTickerActive)
    {
        TickerHandle = FTSTicker::GetCoreTicker().AddTicker(
            FTickerDelegate::CreateUObject(this, &UGameInfoWidget::HandleTick));
        bTickerActive = true;
    }
}

void UGameInfoWidget::PlayAppearFromCenter()
{
    BeginAnim(EMode::Appearing, AppearDuration);
}

void UGameInfoWidget::PlayDisappearToCenter()
{
    BeginAnim(EMode::Disappearing, DisappearDuration);
}

bool UGameInfoWidget::HandleTick(float DeltaTime)
{
    if (Mode == EMode::None) { bTickerActive = false; return false; }

    Elapsed += DeltaTime;
    const float t = FMath::Clamp(Elapsed / Duration, 0.f, 1.f);

    if (Mode == EMode::Appearing)
    {
        const float e = EaseOut(t);
        const float sc = FMath::Lerp(StartScale, 1.f, e);
        const float op = bFadeOpacity ? e : 1.f;
        const float oy = bUseSubtleMove ? FMath::Lerp(-8.f, 0.f, e) : 0.f;
        Apply(sc, op, oy);

        if (t >= 1.f)
        {
            Apply(1.f, 1.f, 0.f);
            Mode = EMode::None;
            bTickerActive = false;
            return false;
        }
    }
    else // Disappearing
    {
        const float e = EaseIn(t);
        const float sc = FMath::Lerp(1.f, StartScale, e);
        const float op = bFadeOpacity ? (1.f - e) : 1.f;
        const float oy = bUseSubtleMove ? FMath::Lerp(0.f, -8.f, e) : 0.f;
        Apply(sc, op, oy);

        if (t >= 1.f)
        {
            Apply(StartScale, 0.f, bUseSubtleMove ? -8.f : 0.f);
            SetVisibility(ESlateVisibility::Collapsed);
            Mode = EMode::None;
            bTickerActive = false;
            return false;
        }
    }

    return true;
}

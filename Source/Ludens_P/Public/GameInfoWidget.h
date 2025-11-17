// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Containers/Ticker.h"
#include "GameInfoWidget.generated.h"

class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSlideFinished);

UCLASS()
class LUDENS_P_API UGameInfoWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CenterAppear")
    float AppearDuration = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CenterAppear")
    float DisappearDuration = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CenterAppear")
    bool bFadeOpacity = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CenterAppear")
    bool bUseSubtleMove = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CenterAppear")
    float StartScale = 0.5f;

    UFUNCTION(BlueprintCallable, Category = "CenterAppear")
    void PlayAppearFromCenter();

    UFUNCTION(BlueprintCallable, Category = "CenterAppear")
    void PlayDisappearToCenter();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX")
    USoundBase* WidgetSFX = nullptr;

    UFUNCTION(BlueprintCallable, Category = "UI")
    void PlaySFX();

protected:
    virtual void NativeDestruct() override;

private:
    enum class EMode : uint8 { None, Appearing, Disappearing };
    EMode Mode = EMode::None;

    FTSTicker::FDelegateHandle TickerHandle;
    bool bTickerActive = false;

    float Elapsed = 0.f;
    float Duration = 0.f;

    void Apply(float Scale, float Opacity, float OffsetY);
    bool HandleTick(float DeltaTime);
    float EaseOut(float T) const { float u = 1.f - T; return 1.f - u * u * u; }
    float EaseIn(float T) const { return T * T * T; }

    void BeginAnim(EMode NewMode, float DurationSec);
};
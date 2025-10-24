// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class LUDENS_P_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget)) UTextBlock* ConfirmLabel;        // 텍스트 교체용
    UPROPERTY(meta = (BindWidget)) UButton* ConfirmButton;          // 버튼 자체

    UFUNCTION(BlueprintCallable) void SetConfirmWaiting();        // "대기중" 텍스트로 교체

protected:
    virtual void NativeOnInitialized() override;

private:
    UFUNCTION() void HandleConfirmClicked();                      // 버튼 클릭 콜백
    bool bLocalConfirmed = false;                                 // 중복 클릭 방지
};

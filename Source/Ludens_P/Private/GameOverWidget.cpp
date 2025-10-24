// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Ludens_P/Ludens_PCharacter.h"
#include "Ludens_P/Ludens_PPlayerController.h"

void UGameOverWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    if (ConfirmLabel)
    {
        ConfirmLabel->SetText(FText::FromString(TEXT("Continue..?")));
    }

    if (ConfirmButton)
    {
        // 위젯 내부에서 직접 바인딩
        ConfirmButton->OnClicked.AddDynamic(this, &UGameOverWidget::HandleConfirmClicked);
    }
}

void UGameOverWidget::HandleConfirmClicked()
{
    if (bLocalConfirmed) return;            // 중복 방지
    bLocalConfirmed = true;

    // 1) 텍스트 "대기중"으로 바꾸고 버튼 비활성화(위젯은 닫지 않음)
    SetConfirmWaiting();
    if (ConfirmButton) ConfirmButton->SetIsEnabled(false);

    // 2) 내 PlayerController에 '확인' 알림
    if (auto* PC = GetOwningPlayer<ALudens_PPlayerController>())
    {
        PC->OnGameOverConfirmClicked();     // 서버 RPC로 합의 집계 진행
    }
}

void UGameOverWidget::SetConfirmWaiting()
{
    if (ConfirmLabel)
    {
        ConfirmLabel->SetText(FText::FromString(TEXT("Waiting...")));
    }
}

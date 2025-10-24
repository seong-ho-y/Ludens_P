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
        // ���� ���ο��� ���� ���ε�
        ConfirmButton->OnClicked.AddDynamic(this, &UGameOverWidget::HandleConfirmClicked);
    }
}

void UGameOverWidget::HandleConfirmClicked()
{
    if (bLocalConfirmed) return;            // �ߺ� ����
    bLocalConfirmed = true;

    // 1) �ؽ�Ʈ "�����"���� �ٲٰ� ��ư ��Ȱ��ȭ(������ ���� ����)
    SetConfirmWaiting();
    if (ConfirmButton) ConfirmButton->SetIsEnabled(false);

    // 2) �� PlayerController�� 'Ȯ��' �˸�
    if (auto* PC = GetOwningPlayer<ALudens_PPlayerController>())
    {
        PC->OnGameOverConfirmClicked();     // ���� RPC�� ���� ���� ����
    }
}

void UGameOverWidget::SetConfirmWaiting()
{
    if (ConfirmLabel)
    {
        ConfirmLabel->SetText(FText::FromString(TEXT("Waiting...")));
    }
}

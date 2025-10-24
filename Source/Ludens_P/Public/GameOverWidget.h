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
    UPROPERTY(meta = (BindWidget)) UTextBlock* ConfirmLabel;        // �ؽ�Ʈ ��ü��
    UPROPERTY(meta = (BindWidget)) UButton* ConfirmButton;          // ��ư ��ü

    UFUNCTION(BlueprintCallable) void SetConfirmWaiting();        // "�����" �ؽ�Ʈ�� ��ü

protected:
    virtual void NativeOnInitialized() override;

private:
    UFUNCTION() void HandleConfirmClicked();                      // ��ư Ŭ�� �ݹ�
    bool bLocalConfirmed = false;                                 // �ߺ� Ŭ�� ����
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardSystemComponent.h"
#include "RewardEffect.h"
#include "RewardUIWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

// Sets default values
URewardSystemComponent::URewardSystemComponent()
{
	SetIsReplicatedByDefault(true);
}

bool URewardSystemComponent::GetRowData(FName RowName, FRewardRow& Out) const
{
	UDataTable* DT = RewardTable.IsValid() ? RewardTable.Get() : RewardTable.LoadSynchronous();
	if (!DT) return false;
	if (const FRewardRow* Found = DT->FindRow<FRewardRow>(RowName, TEXT("RewardLookup")))
	{
		Out = *Found; return true;
	}
	return false;
}

void URewardSystemComponent::ApplyReward(const FRewardRow& Row)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());

	URewardEffect* Effect = NewObject<URewardEffect>(this, URewardEffect::StaticClass());
	if (Effect) Effect->ApplyReward(OwnerCharacter, Row);
}

/* ����: ���� 3��(���� 0~2) �̱� �� �ش� �÷��̾� Ŭ�󿡸� ���� */
void URewardSystemComponent::Server_ShowRewardOptions_Implementation()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	UDataTable* DT = RewardTable.IsValid() ? RewardTable.Get() : RewardTable.LoadSynchronous();
	if (!DT) return;

	// �ĺ� ������ ������ 3��(�ߺ� ����)
	TArray<FName> All = DT->GetRowNames();
	if (All.Num() == 0) return;
	
	LastOfferedRowNames.Reset();
	for (int32 i = 0; i < 3 && All.Num() > 0; ++i)
	{
		int32 Idx = FMath::RandRange(0, All.Num() - 1);
		LastOfferedRowNames.Add(All[Idx]);
		All.RemoveAtSwap(Idx);
	}

	Client_ShowRewardUI(LastOfferedRowNames);
}

/* Ŭ��: ���� �ε����� ���� AllRewards���� �����͸� ���� �� UI ���� */
void URewardSystemComponent::Client_ShowRewardUI_Implementation(const TArray<FName>& OptionRowNames)
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController());
	if (!OwnerChar || !PC) return;
	
	if (!RewardUIClass) return;

	TArray<FRewardUIData> UIList;
	for (FName RowName : OptionRowNames)
	{
		FRewardRow Row;
		if (GetRowData(RowName, Row))
		{
			FRewardUIData D;
			D.RowName = RowName;
			D.Title = Row.RewardName;
			D.Body = Row.Description;
			D.Icon = Row.Icon.IsNull() ? nullptr : Row.Icon.LoadSynchronous();
			UIList.Add(D);
		}
	}

	if (UIList.Num() == 0) return;

	if (ActiveRewardWidget)
	{
		ActiveRewardWidget->RemoveFromParent();
		ActiveRewardWidget = nullptr;
	}

	ActiveRewardWidget = CreateWidget<URewardUIWidget>(PC, RewardUIClass);
	if (!ActiveRewardWidget) return;

	ActiveRewardWidget->AddToViewport();					// ������ Server_SelectReward ȣ���� ��ü
	ActiveRewardWidget->InitWithRows(OwnerChar, UIList);	// ����/����/������ ä���       

	// �Է� ���(Ŭ��)
	PC->SetIgnoreMoveInput(true);
	PC->SetIgnoreLookInput(true);
}

/* ����: �÷��̾ ���� ����(0/1/2)�� ���� �� ���� ���� ���� */
void URewardSystemComponent::Server_SelectReward_Implementation(FName PickedRowName)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	if (!LastOfferedRowNames.Contains(PickedRowName)) return;

	FRewardRow Row;
	if (!GetRowData(PickedRowName, Row)) return;
	ApplyReward(Row);					// ���� ���� ���� (�̹� �����Ƿ� �״�� ���)

	LastOfferedRowNames.Reset();		// �� �� ��������� ���
	Client_EnableInputAfterReward();	// UI �ݱ� & �Է� ������ �ش� �÷��̾� Ŭ�󿡼� ó��
}

void URewardSystemComponent::Client_EnableInputAfterReward_Implementation()
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController());
	if (!PC) return;

	PC->SetIgnoreMoveInput(false);
	PC->SetIgnoreLookInput(false);
	PC->bShowMouseCursor = false;
	PC->SetInputMode(FInputModeGameOnly());

	if (ActiveRewardWidget)
	{
		ActiveRewardWidget->RemoveSelf();
		ActiveRewardWidget = nullptr;
	}

	PC->SetIgnoreMoveInput(false);
	PC->SetIgnoreLookInput(false);
}
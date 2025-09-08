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

/* ����: ���� 3��(���� 0~2) �̱� �� �ش� �÷��̾� Ŭ�󿡸� ���� */
void URewardSystemComponent::Server_ShowRewardOptions_Implementation()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	const int32 NumToOffer = 3;
	if (AllRewards.Num() < NumToOffer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough rewards in AllRewards (need %d)."), NumToOffer);
		return;
	}

	// �ߺ� ���� 3�� �ε��� ����
	LastOfferedIndices.Reset();
	FRandomStream RStream(FMath::Rand());
	while (LastOfferedIndices.Num() < NumToOffer)
	{
		const int32 Pick = RStream.RandRange(0, AllRewards.Num() - 1);
		if (!LastOfferedIndices.Contains(Pick))
		{
			LastOfferedIndices.Add(Pick);
		}
	}

	// ���� ���� �̸��� ���� ���
	FString Names;
	for (int32 Idx : LastOfferedIndices)
	{
		if (AllRewards.IsValidIndex(Idx))
		{
			Names += FString::Printf(TEXT("[%d]%s "), Idx, *AllRewards[Idx].RewardName.ToString());
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[Server_ShowRewardOptions] Owner=%s  Offer=%s"), *Owner->GetName(), *Names);

	// �ش� �÷��̾��� Ŭ�󿡸� UI ���� ����(�ε����� ����)
	Client_ShowRewardUI(LastOfferedIndices);
}

/* Ŭ��: ���� �ε����� ���� AllRewards���� �����͸� ���� �� UI ���� */
void URewardSystemComponent::Client_ShowRewardUI_Implementation(const TArray<int32>& OptionIndices)
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController());
	if (!PC) return;

	if (!RewardUIClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("RewardUIClass not set."));
		return;
	}

	// �ε��� �� ���� FRewardData �迭�� ��ȯ
	TArray<FRewardData> Choices;
	Choices.Reserve(OptionIndices.Num());
	FString Names;
	for (int32 Idx : OptionIndices)
	{
		if (AllRewards.IsValidIndex(Idx))
		{
			Choices.Add(AllRewards[Idx]);
			Names += FString::Printf(TEXT("[%d]%s "), Idx, *AllRewards[Idx].RewardName.ToString());
		}
	}
	if (Choices.Num() == 0) return;
	UE_LOG(LogTemp, Log, TEXT("[Client_ShowRewardUI] PC=%s  Received=%s"), *PC->GetName(), *Names);

	// ���� ���� ����
	if (ActiveRewardWidget)
	{
		ActiveRewardWidget->RemoveFromParent();
		ActiveRewardWidget = nullptr;
	}

	ActiveRewardWidget = CreateWidget<URewardUIWidget>(PC, RewardUIClass);
	if (!ActiveRewardWidget) return;

	ActiveRewardWidget->AddToViewport();
	ActiveRewardWidget->SetOwnerPlayer(OwnerChar);     // ������ Server_SelectReward ȣ���� ��ü
	ActiveRewardWidget->SetRewardList(Choices);        // ����/����/������ ä���

	// �Է� ���(Ŭ��)
	PC->SetIgnoreMoveInput(true);
	PC->SetIgnoreLookInput(true);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan, TEXT("Reward UI Shown (Client-only)"));
	}
}

/* ����: �÷��̾ �� ����(0/1/2)�� ���� �� ���� ���� ���� */
void URewardSystemComponent::Server_SelectReward_Implementation(int32 ChoiceSlotIndex)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	if (!LastOfferedIndices.IsValidIndex(ChoiceSlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid ChoiceSlotIndex: %d"), ChoiceSlotIndex);
		return;
	}

	const int32 RewardIdx = LastOfferedIndices[ChoiceSlotIndex];
	if (!AllRewards.IsValidIndex(RewardIdx))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid RewardIdx mapped from slot: %d"), RewardIdx);
		return;
	}

	const FRewardData& Picked = AllRewards[RewardIdx];
	ApplyReward(Picked);                 // ���� ���� ���� (�̹� �����Ƿ� �״�� ���)

	UE_LOG(LogTemp, Log, TEXT("[Server_SelectReward] Owner=%s  Slot=%d -> Idx=%d (%s)  Applying..."),
		*Owner->GetName(), ChoiceSlotIndex, RewardIdx, *Picked.RewardName.ToString());

	LastOfferedIndices.Reset();          // �� �� ��������� ���

	// UI �ݱ� & �Է� ������ �ش� �÷��̾� Ŭ�󿡼� ó��
	Client_EnableInputAfterReward();
}

void URewardSystemComponent::ApplyReward(const FRewardData& Data)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !Data.EffectClass) return;

	UE_LOG(LogTemp, Log, TEXT("[ApplyReward] Owner=%s  EffectClass=%s"),
		*OwnerCharacter->GetName(),
		*Data.EffectClass->GetName());

	URewardEffect* Effect = NewObject<URewardEffect>(this, Data.EffectClass);
	if (Effect) { Effect->ApplyReward(OwnerCharacter); }
}

void URewardSystemComponent::Client_EnableInputAfterReward_Implementation()
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Still freezed..."));
		}
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController());
	if (!PC)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Still freezed..."));
		}
		return;
	}

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

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Game Unfreeze!"));
	}
}
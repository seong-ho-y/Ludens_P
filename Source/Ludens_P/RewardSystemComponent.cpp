// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardSystemComponent.h"
#include "RewardEffect.h"
#include "RewardUIWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Engine.h"

// Sets default values
URewardSystemComponent::URewardSystemComponent()
{
	SetIsReplicatedByDefault(true);
}

void URewardSystemComponent::ShowRewardOptions()
{
	CurrentChoices.Empty();

	while (CurrentChoices.Num() < 3 && AllRewards.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, AllRewards.Num() - 1);
		CurrentChoices.Add(AllRewards[Index]);
	}

	// TODO: 보상 UI에 CurrentChoices 전달

	// 로그 출력 (에디터 Output Log)
	for (int32 i = 0; i < CurrentChoices.Num(); ++i)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				5.f,
				FColor::Green,
				FString::Printf(TEXT("Reward %d: %s"), i, *CurrentChoices[i].RewardName.ToString())
			);
		}
	}

	// UI 생성은 Multicast로 클라이언트에게 요청
	Multicast_ShowRewardUI();
}

void URewardSystemComponent::Server_SelectReward_Implementation(int32 Index)
{
	if (!CurrentChoices.IsValidIndex(Index)) return;

	const FRewardData& SelectedReward = CurrentChoices[Index];
	ApplyReward(SelectedReward);

	if (GEngine)
	{
		FString Msg = FString::Printf(TEXT("Reward Choosed: %s"), *SelectedReward.RewardName.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Msg);
	}

	// 선택 후 목록 초기화 (중복 선택 방지)
	CurrentChoices.Empty();

	Client_EnableInputAfterReward();
}

void URewardSystemComponent::ApplyReward(const FRewardData& Data)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter || !Data.EffectClass) return;

	URewardEffect* Effect = NewObject<URewardEffect>(this, Data.EffectClass);
	if (Effect) { Effect->ApplyReward(OwnerCharacter); }
}

void URewardSystemComponent::Multicast_ShowRewardUI_Implementation()
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController());
	if (!PC || !PC->IsLocalController()) return;  // 반드시 로컬 컨트롤러만 실행

	if (!RewardUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("RewardUIClass is null!"));
		return;
	}

	URewardUIWidget* Widget = CreateWidget<URewardUIWidget>(PC, RewardUIClass);
	if (Widget)
	{
		Widget->AddToViewport();
		Widget->SetRewardList(CurrentChoices);
		Widget->SetOwnerPlayer(OwnerChar);

		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}

	ActiveRewardWidget = Widget;  // UI 띄운 후 저장
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

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Game Unfreeze!"));
	}
}
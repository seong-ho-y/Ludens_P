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

	if (Row.TargetRewardType == ERewardType::Stat)
	{
		URewardEffect_Stat* Effect = NewObject<URewardEffect_Stat>(this, URewardEffect_Stat::StaticClass());
		if (Effect) Effect->ApplyReward(OwnerCharacter, Row);
	}
	else if (Row.TargetRewardType == ERewardType::Skill)
	{
		URewardEffect_Skill* Effect = NewObject<URewardEffect_Skill>(this, URewardEffect_Skill::StaticClass());
		if (Effect) Effect->ApplyReward(OwnerCharacter, Row);
	}
	else return;
}

void URewardSystemComponent::Server_ShowRewardOptions_Implementation()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	UDataTable* DT = RewardTable.IsValid() ? RewardTable.Get() : RewardTable.LoadSynchronous();
	if (!DT) return;

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

	ActiveRewardWidget->AddToViewport();
	ActiveRewardWidget->InitWithRows(OwnerChar, UIList);

	// 입력 차단: 게임 입력을 UI로 전환
	PC->SetShowMouseCursor(true);

	FInputModeUIOnly Mode;
	Mode.SetWidgetToFocus(ActiveRewardWidget->TakeWidget());
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PC->SetInputMode(Mode);

	// 이동/시야 이그노어 + 캐릭터 입력 자체 비활성화(발사 등 안전차단)
	PC->SetIgnoreMoveInput(true);
	PC->SetIgnoreLookInput(true);
	OwnerChar->DisableInput(PC);
}

void URewardSystemComponent::Server_SelectReward_Implementation(FName PickedRowName)
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	if (!LastOfferedRowNames.Contains(PickedRowName)) return;

	FRewardRow Row;
	if (!GetRowData(PickedRowName, Row)) return;
	ApplyReward(Row);

	LastOfferedRowNames.Reset();
	Client_EnableInputAfterReward();
}

void URewardSystemComponent::Client_EnableInputAfterReward_Implementation()
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController());
	if (!PC) return;

	if (ActiveRewardWidget)
	{
		ActiveRewardWidget->RemoveSelf();
		ActiveRewardWidget = nullptr;
	}

	// 입력 원복
	PC->SetShowMouseCursor(false);
	PC->SetInputMode(FInputModeGameOnly());
	PC->SetIgnoreMoveInput(false);
	PC->SetIgnoreLookInput(false);
	OwnerChar->EnableInput(PC);
}
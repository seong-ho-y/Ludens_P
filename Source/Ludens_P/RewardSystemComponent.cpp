// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardSystemComponent.h"
#include "RewardEffect.h"
#include "RewardUIWidget.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "MyGameInstance.h"
#include "Engine/Engine.h"

TArray<FRewardData> URewardSystemComponent::MasterRewardList;
bool URewardSystemComponent::bIsMasterListLoaded = false;

void URewardSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	if (!GetOwner()->HasAuthority()) return;
	if (!bIsMasterListLoaded)
	{
		if (RewardDataTable)
		{
			TArray<FName> RowNames = RewardDataTable->GetRowNames();
			for (const FName& RowName : RowNames)
			{
				// 각 행 이름으로 실제 데이터(FRewardData)를 찾아옵니다.
				FRewardData* RowData = RewardDataTable->FindRow<FRewardData>(RowName, TEXT(""));
				UE_LOG(LogTemp, Warning, TEXT("RowName : %p"),RowData);
				if (RowData)
				{
					MasterRewardList.Add(*RowData);
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("=== MASTER REWARD LIST HAS BEEN LOADED ONCE. Count: %d ==="), MasterRewardList.Num());
	}
	AllRewards = MasterRewardList;
}

// Sets default values
URewardSystemComponent::URewardSystemComponent()
{
	SetIsReplicatedByDefault(true);
}
// 새로운 함수: 서버에서만 호출되어, 자신(컴포넌트의 오너)에게 보상을 보냄
void URewardSystemComponent::GenerateAndShowRewardsForOwner()
{
	CurrentChoices.Empty(); 

	while (CurrentChoices.Num() < 3 && AllRewards.Num() > 0)
	{
		int32 Index = FMath::RandRange(0, AllRewards.Num() - 1);
		CurrentChoices.Add(AllRewards[Index]); // 지역 변수가 아닌 멤버 변수에 직접 추가
	}
    
	// ShowRewardOptions에 있던 화면 디버그 로그를 여기에 추가!
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

	UE_LOG(LogTemp, Error, TEXT("SERVER: Attempting to send Client_ShowRewardUI to player. Choices Count: %d"), CurrentChoices.Num());
	Client_ShowRewardUI(CurrentChoices);
}

// 기존 Multicast 함수를 Client RPC로 변경하고, 파라미터를 받도록 수정
void URewardSystemComponent::Client_ShowRewardUI_Implementation(const TArray<FRewardData>& RewardChoices)
{
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (!OwnerChar) return;

	APlayerController* PC = Cast<APlayerController>(OwnerChar->GetController());
	// 로컬 컨트롤러인지 확인하는 것은 Client RPC에서 필수
	if (!PC || !PC->IsLocalController()) return;

	if (!RewardUIClass)
	{
		UE_LOG(LogTemp, Error, TEXT("RewardUIClass is null!"));
		return;
	}

	URewardUIWidget* Widget = CreateWidget<URewardUIWidget>(PC, RewardUIClass);
	if (Widget)
	{
		Widget->AddToViewport();
		// 서버로부터 받은 이 플레이어만의 보상 목록을 UI에 설정
		Widget->SetRewardList(RewardChoices);
		Widget->SetOwnerPlayer(OwnerChar);

		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}

	ActiveRewardWidget = Widget;
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
	if (!PC || !PC->IsLocalController()) return;

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

	ActiveRewardWidget = Widget;
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
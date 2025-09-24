// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardEffect.h"
#include "RewardData.h"

void URewardEffect::ApplyReward_Implementation(ACharacter* Player, const FRewardRow& Row)
{
	/*
	UE_LOG(LogTemp, Display, TEXT("Apply Reward Stat"));
	if (!TargetCharacter) return;

	// 캐릭터의 능력치 관리 컴포넌트를 가져오는 것을 추천합니다.
	// UYourCharacterStatComponent* StatComp = TargetCharacter->FindComponentByClass<UYourCharacterStatComponent>();
	// if (!StatComp) return;

	UPlayerStateComponent* StatComp = TargetCharacter->FindComponentByClass<UPlayerStateComponent>();
	if (!StatComp)
	{
		UE_LOG(LogTemp, Error, TEXT("Required Component not found"));
		return;
	}
	// StatToModify 값에 따라 다른 능력치를 변경
	switch (StatToModify)
	{
	case EPlayerStat::MaxHealth:
		if (ModificationType==EStatModificationType::Add) StatComp->MaxHP += Value;
		else StatComp->MaxHP *= Value;
		UE_LOG(LogTemp, Display, TEXT("MaxHP = %f"), StatComp->MaxHP);
		break;

	case EPlayerStat::MoveSpeed:
		if (ModificationType==EStatModificationType::Add) StatComp->MoveSpeed += Value;
		else StatComp->MoveSpeed *= Value;
		UE_LOG(LogTemp, Display, TEXT("Max Speed = %f"), StatComp->MoveSpeed);
		break;

	case EPlayerStat::AttackDamage:
		// 예시: StatComp->AttackDamage += Value;
			UE_LOG(LogTemp, Log, TEXT("AttackDamage changed by %f"), Value);
		break;
    
	case EPlayerStat::ReloadSpeed:
		// 재장전 시간 '감소'는 양수 값을 더하는게 아니라, 기존 값에서 빼줘야 함.
			// 또는 곱연산으로 처리 (예: 0.9를 곱하면 10% 감소)
				// 예시: StatComp->ReloadTime *= Value; // Value에 0.9 등을 넣음
					UE_LOG(LogTemp, Log, TEXT("ReloadSpeed changed by %f"), Value);
		break;

		// ... (위에 정의한 다른 모든 스탯들에 대해서도 같은 방식으로 추가)
    
	default:
		break;
	}
	*/
}

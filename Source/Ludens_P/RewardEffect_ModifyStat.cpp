#include "RewardEffect_ModifyStat.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
// #include "YourCharacterStatComponent.h" // 캐릭터 능력치 컴포넌트가 있다면 포함

void URewardEffect_ModifyStat::ApplyReward_Implementation(ACharacter* TargetCharacter)
{
	if (!TargetCharacter) return;

	// 캐릭터의 능력치 관리 컴포넌트를 가져오는 것을 추천합니다.
	// UYourCharacterStatComponent* StatComp = TargetCharacter->FindComponentByClass<UYourCharacterStatComponent>();
	// if (!StatComp) return;

	// StatToModify 값에 따라 다른 능력치를 변경
	switch (StatToModify)
	{
	case EPlayerStat::MaxHealth:
		// 예시: StatComp->MaxHealth += Value;
			UE_LOG(LogTemp, Log, TEXT("MaxHealth changed by %f"), Value);
		break;

	case EPlayerStat::MoveSpeed:
		if (UCharacterMovementComponent* MoveComp = TargetCharacter->GetCharacterMovement())
		{
			if (ModificationType == EStatModificationType::Add)
			{
				MoveComp->MaxWalkSpeed += Value;
			}
			else // Multiply
			{
				MoveComp->MaxWalkSpeed *= Value;
			}
		}
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
}
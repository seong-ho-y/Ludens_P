#pragma once

#include "CoreMinimal.h"
#include "RewardEffect.h"
#include "RewardEffect_ModifyStat.generated.h"

// 어떤 능력치를 변경할지 정의하는 열거형(Enum)
UENUM(BlueprintType)
enum class EPlayerStat : uint8
{
	MaxHealth,
	MaxShield,
	ShieldRecoverySpeed,
	MoveSpeed,
	DashChargeSpeed,
	MaxDashCount,
	JumpHeight,
	DownedSurvivalTime, // 팀 기절 생존시간
	AttackDamage,
	AttackSpeed,
	AttackRange,
	ReloadSpeed, // 재장전 시간은 '감소'되므로 값은 음수로 넣으면 됨
	CriticalChance,
	CriticalDamage,
	JelluAbsorbSpeed, // 젤루 흡수 속도
	MaxJelluCount,
	BulletSpeed,
	MagazineSize,
	ToolCooldown // 도구 쿨타임
};

// 능력치를 어떻게 변경할지 (더하기 or 곱하기) 정의하는 열거형
UENUM(BlueprintType)
enum class EStatModificationType : uint8
{
	Add,      // 값만큼 더하기 (e.g., 체력 +10)
	Multiply  // 비율만큼 곱하기 (e.g., 공격력 +10% -> 1.1 곱하기)
};

UCLASS()
class LUDENS_P_API URewardEffect_ModifyStat : public URewardEffect
{
	GENERATED_BODY()

public:
	// 블루프린트에서 설정할 변수들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward Effect")
	EPlayerStat StatToModify;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward Effect")
	EStatModificationType ModificationType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward Effect")
	float Value;

	// 부모의 ApplyReward 함수를 오버라이드해서 실제 로직을 구현
	virtual void ApplyReward_Implementation(ACharacter* TargetCharacter) override;
};
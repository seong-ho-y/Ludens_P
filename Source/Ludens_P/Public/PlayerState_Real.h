// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Components/ActorComponent.h"
#include "Ludens_P/EEnemyColor.h"
#include "Net/UnrealNetwork.h"
#include "Ludens_P/RewardData.h"
#include "PlayerState_Real.generated.h"

/**
 * 
 */
UCLASS()
class LUDENS_P_API APlayerState_Real : public APlayerState
{
	GENERATED_BODY()
	
public:
	APlayerState_Real();
	
	//플레이어 색
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerColor, Category = "Player")
	EEnemyColor PlayerColor;
	
	UPROPERTY(Replicated) float MaxHP = 100.f;
	UPROPERTY(Replicated) float MaxShield = 100.f;
	UPROPERTY(Replicated) float MoveSpeed = 1000.f;
	UPROPERTY(Replicated) float ShieldRegenSpeed = 0.5f; // 쉴드 재생 속도
	UPROPERTY(Replicated) float DashRechargeTime = 3.f; // 대쉬 재생 속도
	UPROPERTY(Replicated) int MaxDashCount = 3; // 대쉬 수
	UPROPERTY(Replicated) float AttackDamage = 30.f; // 프로젝타일 공격력 ✅
	UPROPERTY(Replicated) float WeaponAttackCoolTime = 0.3f; // 공격속도 ✅
	UPROPERTY(Replicated) float CriticalRate = 0.1f; // ❌
	UPROPERTY(Replicated) float CriticalDamage = 1.5f; // ❌
	UPROPERTY(Replicated) float AbsorbDelay = 0.5f; // 젤루 흡수 딜레이 ✅
	UPROPERTY(Replicated) int MaxSavedAmmo = 500; // 최대 저장 가능한 탄알(젤루) ✅
	UPROPERTY(Replicated) int MaxAmmo = 15; // 최대 재장전 가능한 탈알(젤루) ✅

protected:
	virtual void BeginPlay() override;

	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OverrideWith(APlayerState* PlayerState) override;

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

protected:
	UFUNCTION() void OnRep_PlayerColor();

public:
	// ---- PSR: 영속/공유 스탯 변경 메서드들
	UFUNCTION() void ApplyMoveSpeed(ERewardOpType Op, float V);
	UFUNCTION() void ApplyDashRechargeTime(ERewardOpType Op, float V);
	UFUNCTION() void ApplyMaxDashCount(ERewardOpType Op, float V);     // 내부에서 정수화

	UFUNCTION() void ApplyAttackDamage(ERewardOpType Op, float V);
	UFUNCTION() void ApplyWeaponAttackCoolTime(ERewardOpType Op, float V); // 낮을수록 빠름(곱 권장)
	UFUNCTION() void ApplyCriticalRate(ERewardOpType Op, float V);         // 0~1 클램프
	UFUNCTION() void ApplyCriticalDamage(ERewardOpType Op, float V);       // 하한 클램프(예: 1.0)

	UFUNCTION() void ApplyAbsorbDelay(ERewardOpType Op, float V);
	UFUNCTION() void ApplyMaxSavedAmmo(ERewardOpType Op, float V);    // 내부에서 정수화
	UFUNCTION() void ApplyMaxAmmo(ERewardOpType Op, float V);         // 내부에서 정수화
};

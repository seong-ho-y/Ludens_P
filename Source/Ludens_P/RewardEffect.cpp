// Fill out your copyright notice in the Description page of Project Settings.


#include "RewardEffect.h"
#include "RewardData.h"
#include "PlayerState_Real.h"
#include "PlayerStateComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void URewardEffect_Stat::ApplyReward_Implementation(ACharacter* Player, const FRewardRow& Row)
{
    if (!Player || !Player->HasAuthority()) return;
    auto* PSR = Player->GetPlayerState<APlayerState_Real>();
    auto* PSC = Player->FindComponentByClass<UPlayerStateComponent>();
    if (!PSR && !PSC) return;

    const ERewardOpType Op = Row.Op;
    const float V = Row.Value;

    switch (Row.TargetReward)
    {
        // --- 체력/쉴드: 보상 즉시 풀로 회복되도록 Policy를 HealToFull로 적용
    case EPlayerReward::MaxHP:
        if (PSC) PSC->ApplyMaxHP(Op, V, ECurrentHPPolicy::HealToFull);
        break;
    case EPlayerReward::MaxShield:
        if (PSC) PSC->ApplyMaxShield(Op, V, ECurrentHPPolicy::HealToFull);
        break;

        // --- 이동/대쉬 등 PSR 소유 스탯
    case EPlayerReward::MoveSpeed:             if (PSR) PSR->ApplyMoveSpeed(Op, V); break;
    case EPlayerReward::DashRechargeTime:      if (PSR) PSR->ApplyDashRechargeTime(Op, V); break;
    case EPlayerReward::MaxDashCount:          if (PSR) PSR->ApplyMaxDashCount(Op, V); break;

        // --- 전투 스탯 (PSR)
    case EPlayerReward::AttackDamage:          if (PSR) PSR->ApplyAttackDamage(Op, V); break;
    case EPlayerReward::WeaponAttackCoolTime:  if (PSR) PSR->ApplyWeaponAttackCoolTime(Op, V); break;
    case EPlayerReward::CriticalRate:          if (PSR) PSR->ApplyCriticalRate(Op, V); break;
    case EPlayerReward::CriticalDamage:        if (PSR) PSR->ApplyCriticalDamage(Op, V); break;

        // --- 기타 (PSR)
    case EPlayerReward::AbsorbDelay:           if (PSR) PSR->ApplyAbsorbDelay(Op, V); break;
    case EPlayerReward::MaxSavedAmmo:          if (PSR) PSR->ApplyMaxSavedAmmo(Op, V); break;
    case EPlayerReward::MaxAmmo:               if (PSR) PSR->ApplyMaxAmmo(Op, V); break;

    default: break;
    }
}

void URewardEffect_Skill::ApplyReward_Implementation(ACharacter* Player, const FRewardRow& Row)
{

}
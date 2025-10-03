// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState_Real.h"

APlayerState_Real::APlayerState_Real()
{

}

void APlayerState_Real::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerState_Real::OnRep_PlayerColor()
{
	
}

void APlayerState_Real::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerState_Real, MaxHP);
	DOREPLIFETIME(APlayerState_Real, MaxShield);
	DOREPLIFETIME(APlayerState_Real, MoveSpeed);
	DOREPLIFETIME(APlayerState_Real, ShieldRegenSpeed);
	DOREPLIFETIME(APlayerState_Real, DashRechargeTime);
	DOREPLIFETIME(APlayerState_Real, MaxDashCount);
	DOREPLIFETIME(APlayerState_Real, JumpZVelocity);
	DOREPLIFETIME(APlayerState_Real, AttackDamage);
	DOREPLIFETIME(APlayerState_Real, WeaponAttackCoolTime);
	DOREPLIFETIME(APlayerState_Real, CriticalRate);
	DOREPLIFETIME(APlayerState_Real, CriticalDamage);
	DOREPLIFETIME(APlayerState_Real, AbsorbDelay);
	DOREPLIFETIME(APlayerState_Real, MaxSavedAmmo);
	DOREPLIFETIME(APlayerState_Real, MaxAmmo);
}

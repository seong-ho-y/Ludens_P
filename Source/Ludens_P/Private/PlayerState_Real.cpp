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
	// DOREPLIFETIME(APlayerState_Real, JumpZVelocity);
	DOREPLIFETIME(APlayerState_Real, AttackDamage);
	DOREPLIFETIME(APlayerState_Real, WeaponAttackCoolTime);
	DOREPLIFETIME(APlayerState_Real, CriticalRate);
	DOREPLIFETIME(APlayerState_Real, CriticalDamage);
	DOREPLIFETIME(APlayerState_Real, AbsorbDelay);
	DOREPLIFETIME(APlayerState_Real, MaxSavedAmmo);
	DOREPLIFETIME(APlayerState_Real, MaxAmmo);

	///

	DOREPLIFETIME(APlayerState_Real, AppearanceId);
	DOREPLIFETIME(APlayerState_Real, PreviewColor);
	DOREPLIFETIME(APlayerState_Real, SelectedColor);
	DOREPLIFETIME(APlayerState_Real, SubskillId);
	DOREPLIFETIME(APlayerState_Real, bReady);

	// [중요] 상성 색 (서버가 Ready에서 확정해 주입 → 전 클라 복제)
	DOREPLIFETIME(APlayerState_Real, PlayerColor);

	///
}


///

// --- OnRep들: UI/표현 갱신 트리거 ---
void APlayerState_Real::OnRep_AppearanceId() { OnAnyLobbyFieldChanged.Broadcast(); }
void APlayerState_Real::OnRep_PreviewColor() { OnAnyLobbyFieldChanged.Broadcast(); }
void APlayerState_Real::OnRep_SelectedColor() { OnAnyLobbyFieldChanged.Broadcast(); }
void APlayerState_Real::OnRep_SubskillId() { OnAnyLobbyFieldChanged.Broadcast(); }
void APlayerState_Real::OnRep_Ready() { OnAnyLobbyFieldChanged.Broadcast(); }

void APlayerState_Real::NotifyAnyLobbyFieldChanged()
{
	OnAnyLobbyFieldChanged.Broadcast(); // [PS-UNIFY] 서버에서 수동 트리거
}

// PlayerState_Real.cpp
void APlayerState_Real::CopyProperties(APlayerState* PS){
    Super::CopyProperties(PS);
    if (auto* P = Cast<APlayerState_Real>(PS)){
        P->AppearanceId = AppearanceId;
        P->SelectedColor = SelectedColor;
        P->PlayerColor   = PlayerColor;
        P->SubskillId    = SubskillId;
        P->bReady        = bReady;

    }
}

void APlayerState_Real::OverrideWith(APlayerState* PS)
{
	Super::OverrideWith(PS);
	if (auto* R = Cast<APlayerState_Real>(PS))
	{
		PlayerColor = R->PlayerColor;
		AppearanceId = R->AppearanceId;
		SubskillId = R->SubskillId;
	}
}

void APlayerState_Real::SeamlessTravelTo(APlayerState* PS) {
	Super::SeamlessTravelTo(PS);
	if (auto* P = Cast<APlayerState_Real>(PS)) {
		P->AppearanceId = AppearanceId;
		P->SelectedColor = SelectedColor;
		P->PlayerColor = PlayerColor;
		P->SubskillId = SubskillId;
		P->bReady = bReady;
	}
}


///


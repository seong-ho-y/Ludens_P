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


void APlayerState_Real::CopyProperties(APlayerState* PS)
{
	Super::CopyProperties(PS);

	// ? 복사 전 원본 데이터 로그
	UE_LOG(LogTemp, Warning, TEXT("CopyProperties: SOURCE -> Ap: %d, Sel: %d, Ply: %d"),
		AppearanceId, (int)SelectedColor, (int)PlayerColor);

	if (auto* P = Cast<APlayerState_Real>(PS))
	{
		// ? 복사될 대상(새로운 PlayerState)의 초기 상태 로그
		UE_LOG(LogTemp, Warning, TEXT("CopyProperties: TARGET (Before) -> Ap: %d, Sel: %d, Ply: %d"),
			P->AppearanceId, (int)P->SelectedColor, (int)P->PlayerColor);

		// 데이터 복사
		P->AppearanceId = AppearanceId;
		P->SelectedColor = SelectedColor;
		P->PlayerColor = PlayerColor;
		P->SubskillId = SubskillId;
		P->bReady = bReady;

		// ? 복사 후 대상의 최종 상태 로그
		UE_LOG(LogTemp, Warning, TEXT("CopyProperties: TARGET (After) -> Ap: %d, Sel: %d, Ply: %d"),
			P->AppearanceId, (int)P->SelectedColor, (int)P->PlayerColor);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CopyProperties: Failed to cast PlayerState to APlayerState_Real."));
	}
}

/*

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
*/

void APlayerState_Real::SeamlessTravelTo(APlayerState* NewPlayerState)
{
	Super::SeamlessTravelTo(NewPlayerState);
	if (APlayerState_Real* NewRealPS = Cast<APlayerState_Real>(NewPlayerState))
	{
		// 로비에서 인게임으로 정보를 안전하게 복사
		NewRealPS->AppearanceId = AppearanceId;
		NewRealPS->SelectedColor = SelectedColor;
		NewRealPS->PlayerColor = PlayerColor;
		NewRealPS->SubskillId = SubskillId;
		NewRealPS->bReady = bReady;

		NewRealPS->NotifyAnyLobbyFieldChanged();
	}
}


///


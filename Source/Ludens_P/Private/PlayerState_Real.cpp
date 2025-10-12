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

	// [�߿�] �� �� (������ Ready���� Ȯ���� ���� �� �� Ŭ�� ����)
	DOREPLIFETIME(APlayerState_Real, PlayerColor);

	///
}


///

// --- OnRep��: UI/ǥ�� ���� Ʈ���� ---
void APlayerState_Real::OnRep_AppearanceId() { OnAnyLobbyFieldChanged.Broadcast(); }
void APlayerState_Real::OnRep_PreviewColor() { OnAnyLobbyFieldChanged.Broadcast(); }
void APlayerState_Real::OnRep_SelectedColor() { OnAnyLobbyFieldChanged.Broadcast(); }
void APlayerState_Real::OnRep_SubskillId() { OnAnyLobbyFieldChanged.Broadcast(); }
void APlayerState_Real::OnRep_Ready() { OnAnyLobbyFieldChanged.Broadcast(); }

void APlayerState_Real::NotifyAnyLobbyFieldChanged()
{
	OnAnyLobbyFieldChanged.Broadcast(); // [PS-UNIFY] �������� ���� Ʈ����
}


void APlayerState_Real::CopyProperties(APlayerState* PS)
{
	Super::CopyProperties(PS);

	// ? ���� �� ���� ������ �α�
	UE_LOG(LogTemp, Warning, TEXT("CopyProperties: SOURCE -> Ap: %d, Sel: %d, Ply: %d"),
		AppearanceId, (int)SelectedColor, (int)PlayerColor);

	if (auto* P = Cast<APlayerState_Real>(PS))
	{
		// ? ����� ���(���ο� PlayerState)�� �ʱ� ���� �α�
		UE_LOG(LogTemp, Warning, TEXT("CopyProperties: TARGET (Before) -> Ap: %d, Sel: %d, Ply: %d"),
			P->AppearanceId, (int)P->SelectedColor, (int)P->PlayerColor);

		// ������ ����
		P->AppearanceId = AppearanceId;
		P->SelectedColor = SelectedColor;
		P->PlayerColor = PlayerColor;
		P->SubskillId = SubskillId;
		P->bReady = bReady;

		// ? ���� �� ����� ���� ���� �α�
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
		// �κ񿡼� �ΰ������� ������ �����ϰ� ����
		NewRealPS->AppearanceId = AppearanceId;
		NewRealPS->SelectedColor = SelectedColor;
		NewRealPS->PlayerColor = PlayerColor;
		NewRealPS->SubskillId = SubskillId;
		NewRealPS->bReady = bReady;

		NewRealPS->NotifyAnyLobbyFieldChanged();
	}
}


///


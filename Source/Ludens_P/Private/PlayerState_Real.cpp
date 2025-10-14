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
}

static FORCEINLINE void OpApply(float& S, ERewardOpType Op, float V)
{
    if (Op == ERewardOpType::Multiply) S *= V; else S += V;
}

// ����ȭ ��ƿ
static FORCEINLINE int32 ToIntNonNeg(float V) { return FMath::Max(0, FMath::RoundToInt(V)); }

void APlayerState_Real::ApplyMoveSpeed(ERewardOpType Op, float V)
{
    if (!HasAuthority()) return;
    OpApply(MoveSpeed, Op, V);
    MoveSpeed = FMath::Clamp(MoveSpeed, 50.f, 20000.f);
    ForceNetUpdate();
}

void APlayerState_Real::ApplyDashRechargeTime(ERewardOpType Op, float V)
{
    if (!HasAuthority()) return;
    // ������ "����"�� ��(0.9 = 10% ����) ����
    OpApply(DashRechargeTime, Op, V);
    DashRechargeTime = FMath::Clamp(DashRechargeTime, 0.05f, 60.f);
    ForceNetUpdate();
}

void APlayerState_Real::ApplyMaxDashCount(ERewardOpType Op, float V)
{
    if (!HasAuthority()) return;
    float Temp = (float)MaxDashCount;
    OpApply(Temp, Op, V);
    MaxDashCount = FMath::Clamp(ToIntNonNeg(Temp), 0, 10);
    ForceNetUpdate();
}

void APlayerState_Real::ApplyAttackDamage(ERewardOpType Op, float V)
{
    if (!HasAuthority()) return;
    OpApply(AttackDamage, Op, V);
    AttackDamage = FMath::Max(0.f, AttackDamage);
    ForceNetUpdate();
}

void APlayerState_Real::ApplyWeaponAttackCoolTime(ERewardOpType Op, float V)
{
    if (!HasAuthority()) return;
    // �������� ���� �� �� ���� (0.9 = 10% ������)
    OpApply(WeaponAttackCoolTime, Op, V);
    WeaponAttackCoolTime = FMath::Clamp(WeaponAttackCoolTime, 0.03f, 10.f);
    ForceNetUpdate();
}

void APlayerState_Real::ApplyCriticalRate(ERewardOpType Op, float V)
{
    if (!HasAuthority()) return;
    OpApply(CriticalRate, Op, V);
    CriticalRate = FMath::Clamp(CriticalRate, 0.f, 1.f);
    ForceNetUpdate();
}

void APlayerState_Real::ApplyCriticalDamage(ERewardOpType Op, float V)
{
    if (!HasAuthority()) return;
    OpApply(CriticalDamage, Op, V);
    CriticalDamage = FMath::Max(1.0f, CriticalDamage); // ���� 1.0��
    ForceNetUpdate();
}

void APlayerState_Real::ApplyAbsorbDelay(ERewardOpType Op, float V)
{
    if (!HasAuthority()) return;
    OpApply(AbsorbDelay, Op, V);
    AbsorbDelay = FMath::Clamp(AbsorbDelay, 0.f, 10.f);
    ForceNetUpdate();
}

void APlayerState_Real::ApplyMaxSavedAmmo(ERewardOpType Op, float V)
{
    if (!HasAuthority()) return;
    float Temp = (float)MaxSavedAmmo;
    OpApply(Temp, Op, V);
    MaxSavedAmmo = FMath::Clamp(ToIntNonNeg(Temp), 0, 9999);
    ForceNetUpdate();
}

void APlayerState_Real::ApplyMaxAmmo(ERewardOpType Op, float V)
{
    if (!HasAuthority()) return;
    float Temp = (float)MaxAmmo;
    OpApply(Temp, Op, V);
    MaxAmmo = FMath::Clamp(ToIntNonNeg(Temp), 0, 999);
    ForceNetUpdate();
}

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

    if (APlayerState_Real* PS = Cast<APlayerState_Real>(PlayerState))
    {
        PS->MaxHP = MaxHP;
        PS->MaxShield = MaxShield;
        PS->MoveSpeed = MoveSpeed;
        PS->ShieldRegenSpeed = ShieldRegenSpeed;
        PS->DashRechargeTime = DashRechargeTime;
        PS->MaxDashCount = MaxDashCount;
        PS->AttackDamage = AttackDamage;
        PS->WeaponAttackCoolTime = WeaponAttackCoolTime;
        PS->CriticalRate = CriticalRate;
        PS->CriticalDamage = CriticalDamage;
        PS->AbsorbDelay = AbsorbDelay;
        PS->MaxSavedAmmo = MaxSavedAmmo;
        PS->MaxAmmo = MaxAmmo;
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
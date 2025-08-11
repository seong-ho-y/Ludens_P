#include "ShooterCombatComponent.h"
#include <cstdarg> // va_list
#include "EnemyProjectileWeaponComponent.h"

bool UShooterCombatComponent::TryFire(AActor* Target)
{
	if (!CanFire() || !IsValid(Target)) return false;
	BeginBurst(Target); //쏠 수 있으면 BeginBurst 호출
	return true;
}

bool UShooterCombatComponent::CanFire() const
{
	//서버 권한 & Idle 상태에서만 발사 허용
	const AActor* Owner = GetOwner();
	return Owner && Owner->HasAuthority() && State == EFireState::Idle && Weapon && Weapon->IsValidLowLevelFast();
}
//--------------------------TryFire랑 CanFire는 null ref 막기 위한 메서드------------------------

//실행 메서드들 구현
void UShooterCombatComponent::BeginBurst(AActor* Target)
{
	CachedTarget = Target;
	ShotsLeft = FMath::Max(1, BurstCount);

	//첫 발이 실패하면 상태 전환 X
	if (!DoSingleShot())
	{
		CachedTarget = nullptr;
		ShotsLeft = 0;
		return;
	}

	//첫 발이 성공했다면 Burst 모드 진입(연발 or 단발)
	State = EFireState::Burst;

	if (ShotsLeft > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(
			BurstTimer, this, &UShooterCombatComponent::TickBurst,
			FMath::Max(0.01f, BurstInterval), true
		);
	}
	else
	{
		BeginCooldown();
	}
}

void UShooterCombatComponent::TickBurst()
{
	if (!DoSingleShot())
	{
		// 실패 시 즉시 쿨다운으로 이행(상태 꼬임 방지)
		BeginCooldown();
		return;
	}

	if (ShotsLeft <= 0)
	{
		BeginCooldown();
	}
}

bool UShooterCombatComponent::DoSingleShot()
{
	if (!Weapon) { Log(TEXT("Weapon null")); return false; }
	AActor* Target = CachedTarget.Get();
	if (!IsValid(Target)) { Log(TEXT("Target invalid")); return false; }

	// 실제 스폰은 EnemyWeaponComponent가 서버에서 처리
	Weapon->Server_FireAt(Target);

	ShotsLeft--;
	return true;
}

void UShooterCombatComponent::BeginCooldown()
{
	GetWorld()->GetTimerManager().ClearTimer(BurstTimer);
	State = EFireState::Cooldown;

	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimer, this, &UShooterCombatComponent::EndCooldown,
		FMath::Max(0.01f, FireCooldown), false
	);
}

void UShooterCombatComponent::EndCooldown()
{
	CachedTarget = nullptr;
	State = EFireState::Idle;
}
void UShooterCombatComponent::Log(const TCHAR* Fmt, ...) const
{
	if (!bDebugLog) return;

	TCHAR Buffer[1024];

	va_list Args;
	va_start(Args, Fmt);
	// Dest, DestSize, Format, Args  ← 4개!
	FCString::GetVarArgs(Buffer, UE_ARRAY_COUNT(Buffer), Fmt, Args);
	va_end(Args);

	UE_LOG(LogTemp, Display, TEXT("%s"), Buffer);
}
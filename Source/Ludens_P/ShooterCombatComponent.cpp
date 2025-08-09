#include "ShooterCombatComponent.h"
#include "ShooterEnemyBase.h" // 구현부에서 include

UShooterCombatComponent::UShooterCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false; // 판단은 BT가 하므로 Tick 불필요
}

void UShooterCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerEnemy = Cast<AShooterEnemyBase>(GetOwner());
}

bool UShooterCombatComponent::CanFire() const
{
	if (!OwnerEnemy) return false;
	if (!OwnerEnemy->HasAuthority()) return false; // 서버 권한에서만 발사
	if (bFiring) return false;
	if (GetWorld()->GetTimerManager().IsTimerActive(CooldownHandle)) return false;
	return true;
}

bool UShooterCombatComponent::TryFire(AActor* Target)
{
	UE_LOG(LogTemp, Warning, TEXT("TryFire called. Authority: %d, CanFire: %d, Target: %s"),
	   GetOwner() ? GetOwner()->HasAuthority() : -1,
	   CanFire(),
	   Target ? *Target->GetName() : TEXT("None"));

	
	if (!CanFire() || !IsValid(Target)) return false;

	bFiring = true;
	CachedTarget = Target;
	ShotsLeftInBurst = FMath::Max(1, BurstCount);

	// 첫 발
	DoSingleShot();

	// 남은 발이 있으면 타이머로 연발
	if (ShotsLeftInBurst > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(
			BurstHandle,
			this, &UShooterCombatComponent::DoSingleShot,
			BurstInterval,
			true
		);
	}

	return true;
}

void UShooterCombatComponent::DoSingleShot()
{
	if (!OwnerEnemy || !OwnerEnemy->HasAuthority())
	{
		EndBurst();
		return;
	}

	AActor* Target = CachedTarget.Get();
	if (!IsValid(Target))
	{
		EndBurst();
		return;
	}

	// 실제 발사는 Enemy 쪽 로직을 호출 (서버 RPC 내부에서 프로젝타일 스폰)
	OwnerEnemy->FireAt(Target);

	ShotsLeftInBurst--;
	if (ShotsLeftInBurst <= 0)
	{
		EndBurst();
	}
}

void UShooterCombatComponent::EndBurst()
{
	GetWorld()->GetTimerManager().ClearTimer(BurstHandle);
	GetWorld()->GetTimerManager().SetTimer(
		CooldownHandle,
		this, &UShooterCombatComponent::ResetCooldown,
		FireCooldown,
		false
	);
}

void UShooterCombatComponent::ResetCooldown()
{
	bFiring = false;
}

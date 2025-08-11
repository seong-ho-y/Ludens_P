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
	UE_LOG(LogTemp, Warning, TEXT("ShooterCombat BeginPlay: Owner=%s Shooter=%d"),
		*GetOwner()->GetName(), OwnerEnemy!=nullptr);

	bFiring = false;
	GetWorld()->GetTimerManager().ClearTimer(BurstHandle);
	GetWorld()->GetTimerManager().ClearTimer(CooldownHandle);
}

// 혹시 모를 꼬임 방지: EndBurst/ResetCooldown 로그로 확인
void UShooterCombatComponent::EndBurst()
{
	GetWorld()->GetTimerManager().ClearTimer(BurstHandle);
	GetWorld()->GetTimerManager().SetTimer(
		CooldownHandle, this, &UShooterCombatComponent::ResetCooldown,
		FireCooldown, false
	);
}

void UShooterCombatComponent::ResetCooldown()
{
	bFiring = false;
	CachedTarget = nullptr;
	UE_LOG(LogTemp, Warning, TEXT("ResetCooldown: bFiring=false"));
}

// 워치독(선택): 틱 or 타이머로 가끔 체크
void UShooterCombatComponent::TickComponent(float dt, ELevelTick, FActorComponentTickFunction*)
{
	if (bFiring)
	{
		const bool bNoTimers = !GetWorld()->GetTimerManager().IsTimerActive(BurstHandle)
							&& !GetWorld()->GetTimerManager().IsTimerActive(CooldownHandle);
		if (bNoTimers)
		{
			UE_LOG(LogTemp, Error, TEXT("Watchdog: bFiring stuck -> force reset"));
			bFiring = false;
			CachedTarget = nullptr;
		}
	}
}



bool UShooterCombatComponent::CanFire() const
{
	if (!OwnerEnemy)
	{
		UE_LOG(LogTemp, Error, TEXT("CanFire: OwnerEnemy is null"));
		return false;
	}

	if (!OwnerEnemy->HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire: No authority"));
		return false;
	}

	if (bFiring)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire: Already firing (burst in progress)"));
		return false;
	}

	const bool bOnCooldown = GetWorld()->GetTimerManager().IsTimerActive(CooldownHandle);
	if (bOnCooldown)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire: On cooldown"));
		return false;
	}

	return true;
}

bool UShooterCombatComponent::TryFire(AActor* Target)
{
	UE_LOG(LogTemp, Warning, TEXT("TryFire: Authority=%d, CanFire=%d, Target=%s"),
		GetOwner()->HasAuthority(), CanFire(), Target ? *Target->GetName() : TEXT("None"));

	if (!CanFire() || !IsValid(Target)) return false;

	CachedTarget = Target;
	ShotsLeftInBurst = FMath::Max(1, BurstCount);

	// 첫 발을 먼저 '시도'하고, 성공했을 때만 bFiring을 켠다.
	const bool bFirstShot = DoSingleShot();   // ← 반환값 bool로 만들자(아래 2번)
	if (!bFirstShot)
	{
		UE_LOG(LogTemp, Error, TEXT("TryFire: First shot failed. Abort."));
		CachedTarget = nullptr;
		ShotsLeftInBurst = 0;
		return false;
	}

	bFiring = true;

	if (ShotsLeftInBurst > 0)
	{
		GetWorld()->GetTimerManager().SetTimer(
			BurstHandle, this, &UShooterCombatComponent::DoSingleShotTimerTick,
			BurstInterval, true
		);
	}
	return true;
}


bool UShooterCombatComponent::DoSingleShot()
{
	if (!OwnerEnemy) { UE_LOG(LogTemp, Error, TEXT("DoSingleShot: OwnerEnemy null")); return false; }
	if (!OwnerEnemy->HasAuthority()) { UE_LOG(LogTemp, Warning, TEXT("DoSingleShot: No authority")); return false; }

	AActor* Target = CachedTarget.Get();
	if (!IsValid(Target)) { UE_LOG(LogTemp, Warning, TEXT("DoSingleShot: Target invalid")); return false; }

	UE_LOG(LogTemp, Warning, TEXT("DoSingleShot -> FireAt"));
	OwnerEnemy->FireAt(Target); // (Server RPC 내에서 Spawn)

	// 성공 처리
	ShotsLeftInBurst--;
	if (ShotsLeftInBurst <= 0)
	{
		EndBurst(); // 여기서 Cooldown 타이머 설정 → ResetCooldown에서 bFiring=false
	}
	return true;
}

// 타이머에서 부를 래퍼(시그니처 void)
void UShooterCombatComponent::DoSingleShotTimerTick()
{
	if (!DoSingleShot())
	{
		EndBurst(); // 실패 시 즉시 종료/원복
	}
}
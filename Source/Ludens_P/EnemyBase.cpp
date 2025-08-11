#include "EnemyBase.h"
#include "EnemyDescriptor.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;
	CCC = CreateDefaultSubobject<UCreatureCombatComponent>(TEXT("CreatureCombat"));
}

void AEnemyBase::BeginPlay()
{
	if (Descriptor)
	{
		// 이동속도 적용(너무 낮으면 120 보장)
		GetCharacterMovement()->MaxWalkSpeed = FMath::Max(120.f, Descriptor->WalkSpeed);
	}
	// HP 적용 (컴포넌트에 초기화 메서드가 있어야 깔끔)
	if (CCC)
	{
		CCC->InitStats(Descriptor->MaxHP); // ✨ 이 메서드를 CCC에 추가 권장
	}
	Super::BeginPlay();
	if (CCC)
	{
		CCC->OnDied.AddDynamic(this, &AEnemyBase::HandleDied);
	}
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HasAuthority() && CCC) {CCC->TakeDamage(DamageAmount);}
	return DamageAmount;
}

void AEnemyBase::HandleDied()
{
	// 사망 애니메이션, AI 중지, 캡슐 비활성화, 풀 반환
}

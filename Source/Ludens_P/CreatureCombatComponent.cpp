#include "CreatureCombatComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

UCreatureCombatComponent::UCreatureCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCreatureCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHP = MaxHP;
}

void UCreatureCombatComponent::Attack(AActor* Target)
{
	UE_LOG(LogTemp, Log, TEXT("Attack called on target: %s"), *Target->GetName());

	if (bIsAttacking || bIsDead || !Target) return;

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (AttackMontage && OwnerChar)
	{
		OwnerChar->PlayAnimMontage(AttackMontage);
	}

	// 데미지 전달
	if (UCreatureCombatComponent* TargetCombat = Target->FindComponentByClass<UCreatureCombatComponent>()) //상대의 UCreatureCombatComponent를 찾아서 TakeDamage 호출
	{
		TargetCombat->TakeDamage(Damage);
	}

	bIsAttacking = true;
	GetWorld()->GetTimerManager().SetTimer(AttackCooldownHandle, this, &UCreatureCombatComponent::EndAttack, AttackCooldown, false);
}

void UCreatureCombatComponent::TakeDamage(float Amount)
{
	if (bIsDead) return;

	CurrentHP -= Amount;
	if (CurrentHP <= 0.f)
	{
		Die();
	}
}

void UCreatureCombatComponent::Die()
{
	bIsDead = true;
	UE_LOG(LogTemp, Warning, TEXT("Enemy Died"));
	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (OwnerChar)
	{
		OwnerChar->DetachFromControllerPendingDestroy();
		OwnerChar->SetLifeSpan(5.f);
		OwnerChar->GetMesh()->SetSimulatePhysics(true);
	}
}

void UCreatureCombatComponent::EndAttack()
{
	bIsAttacking = false;
}

float UCreatureCombatComponent::GetHealthPercent() const
{
	return (MaxHP > 0.f) ? CurrentHP / MaxHP : 0.f;
}

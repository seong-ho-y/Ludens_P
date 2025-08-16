#include "CreatureCombatComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

UCreatureCombatComponent::UCreatureCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
}

void UCreatureCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHP = MaxHP;
}



void UCreatureCombatComponent::TakeDamage(float Amount)
{
	if (bIsDead) return;
	CurrentHP -= Amount;
	UE_LOG(LogTemp,Warning,TEXT("Enemy Attackted : %f"),CurrentHP);

	if (CurrentHP <= 0.f)
	{
		Die();
	}
}

void UCreatureCombatComponent::Die()
{
	if (bIsDead) return;
	bIsDead = true;
	OnDied.Broadcast();
}

float UCreatureCombatComponent::GetHealthPercent() const
{
	return (MaxHP > 0.f) ? CurrentHP / MaxHP : 0.f;
}

void UCreatureCombatComponent::InitStats(float InMaxHP)
{
	MaxHP    = FMath::Max(1.f, InMaxHP);
	CurrentHP = MaxHP;
	bIsDead   = false;
}
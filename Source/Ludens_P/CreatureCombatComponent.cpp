#include "CreatureCombatComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

UCreatureCombatComponent::UCreatureCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);
}

void UCreatureCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHP = MaxHP;
}

// ✨ GetLifetimeReplicatedProps 함수를 추가하여 CurrentHP를 복제 등록
void UCreatureCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UCreatureCombatComponent, CurrentHP);
	DOREPLIFETIME(UCreatureCombatComponent, MaxHP);
}


void UCreatureCombatComponent::TakeDamage(float Amount)
{
	if (!GetOwner()->HasAuthority()) return;
	if (bIsDead) return;
	CurrentHP -= Amount;
	UE_LOG(LogTemp,Warning,TEXT("Enemy Attackted : %f, Max HP : %f"),CurrentHP, MaxHP);
	OnRep_CurrentHP();
	if (CurrentHP <= 0.f)
	{
		Die();
	}
}
void UCreatureCombatComponent::OnRep_CurrentHP()
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] CCC::OnRep_CurrentHP fired. Broadcasting OnHealthChanged."), 
		  GetOwner()->HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"));
	// ✨ 체력이 변경되었음을 외부에 알리는 신호를 보냅니다(Broadcast).
	OnHealthChanged.Broadcast(CurrentHP, MaxHP);
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
	OnRep_CurrentHP();
}
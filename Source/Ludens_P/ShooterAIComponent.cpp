// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAIComponent.h"
#include "ShooterEnemyBase.h"

UShooterAIComponent::UShooterAIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
}
void UShooterAIComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerEnemy = Cast<AShooterEnemyBase>(GetOwner());
	WalkerComponent = OwnerEnemy ? OwnerEnemy->FindComponentByClass<UWalkerAIComponent>() : nullptr;
}

void UShooterAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerEnemy || !WalkerComponent || !OwnerEnemy->HasAuthority())
		return;

	AActor* Target = WalkerComponent->FindNearestPlayer();
	if (!Target) return;

	const float Distance = FVector::Dist(Target->GetActorLocation(), OwnerEnemy->GetActorLocation());

	if (Distance <= FireRange)
	{
		WalkerComponent->StopMovement(); // 이동 중지
		TryFire(Target);
	}
	else
	{
		WalkerComponent->StartChasing(Target); // 거리 멀어지면 다시 추적
	}
}

void UShooterAIComponent::TryFire(AActor* Target)
{
	if (!bCanFire || !Target) return;

	OwnerEnemy->FireAt(Target);// 적의 Fire 메서드 호출
	bCanFire = false;

	GetWorld()->GetTimerManager().SetTimer(FireCooldownHandle, this, &UShooterAIComponent::ResetCanFire, FireCooldown, false);
}

void UShooterAIComponent::ResetCanFire()
{
	bCanFire = true;
}

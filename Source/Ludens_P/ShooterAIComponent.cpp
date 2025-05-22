// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAIComponent.h"

#include "EnemyBase.h"

void UShooterAIComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerEnemy = Cast<AEnemyBase>(GetOwner());
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
		TryFire();
	}
	else
	{
		WalkerComponent->StartChasing(Target); // 거리 멀어지면 다시 추적
	}
}

void UShooterAIComponent::TryFire()
{
	if (!bCanFire) return;

	OwnerEnemy->Fire(); // 적의 Fire 메서드 호출
	bCanFire = false;

	GetWorld()->GetTimerManager().SetTimer(FireCooldownHandle, this, &UShooterAIComponent::ResetCanFire, FireCooldown, false);
}

void UShooterAIComponent::ResetCanFire()
{
	bCanFire = true;
}

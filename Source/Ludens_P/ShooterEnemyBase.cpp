// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterEnemyBase.h"

#include "ShooterAIComponent.h"

AShooterEnemyBase::AShooterEnemyBase()
{
	ShooterAIComponent = CreateDefaultSubobject<UShooterAIComponent>(TEXT("ShooterAIComponent"));
	ShootingComponent = CreateDefaultSubobject<UShootingComponent>(TEXT("ShootingComponent"));
}

void AShooterEnemyBase::FireAt(AActor* Target)
{
	UE_LOG(LogTemp, Log, TEXT("FireAt at ShooterEnemy"));
	if (ShootingComponent)
	{
		ShootingComponent->FireAt(Target);
	}
}
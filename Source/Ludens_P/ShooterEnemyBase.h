// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "ShootingComponent.h"
#include "ShooterCombatComponent.h"
#include "ShooterEnemyBase.generated.h"

UCLASS()
class LUDENS_P_API AShooterEnemyBase : public AEnemyBase
{
	GENERATED_BODY()

public:
	AShooterEnemyBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UShootingComponent* ShootingComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UShooterCombatComponent* ShooterCombatComponent;

	UFUNCTION(Server, Reliable)
	void Server_FireAt(AActor* Target);

	UFUNCTION(NetMulticast, UnReliable)
	void Multicast_OnFiredFX();
	
	void FireAt(AActor* Target);
};

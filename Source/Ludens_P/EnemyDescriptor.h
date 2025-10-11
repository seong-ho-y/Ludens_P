// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyDescriptor.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class LUDENS_P_API UEnemyDescriptor : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// === 공통 기본값 ===
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats")
	float MaxHP = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Move")
	float WalkSpeed = 360.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats")
	float MaxShield = 50.f;
	float HealthMultiplier = 2.f;
	float SpeedMultiplier = 1.5f;
	float ShieldMultiplier = 2.f;
};
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sense")
	float DetectRange = 2000.f;

	// === (선택) 슈터용 최소 값 ===
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shooter")
	float AttackRange = 1200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Shooter")
	float AttackRate = 0.8f; // 초당 발사(ROF) 또는 쿨타임의 역수 개념
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHealthBarBase.generated.h"

enum class EEnemyColor : uint8;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FShieldUIData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EEnemyColor ShieldColor;

	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth;
	UPROPERTY(BlueprintReadOnly)
	float MaxHealth;
};
UCLASS()
class LUDENS_P_API UEnemyHealthBarBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateHealthBar(float NewPercent);
	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void UpdateShields(const TArray<FShieldUIData>& ShieldUIData);
};

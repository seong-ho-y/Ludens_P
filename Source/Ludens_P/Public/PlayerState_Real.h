// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Components/ActorComponent.h"
#include "Ludens_P/EEnemyColor.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState_Real.generated.h"

/**
 * 
 */
UCLASS()
class LUDENS_P_API APlayerState_Real : public APlayerState
{
	GENERATED_BODY()
	
public:
	APlayerState_Real();
	virtual void BeginPlay() override;
	
	//플레이어 색
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerColor, Category = "Player")
	EEnemyColor PlayerColor;
	
	UPROPERTY(Replicated)
	float MaxHP = 100.f;;

	UPROPERTY(Replicated)
	float MaxShield = 100.f;;

	UPROPERTY(Replicated)
	float MoveSpeed = 1200.f;;

	UPROPERTY(Replicated)
	float ShieldRegenSpeed = 0.5f;; // 쉴드 재생 속도

	UPROPERTY(Replicated)
	float DashRechargeTime = 3.f;; // 대쉬 재생 속도

	UPROPERTY(Replicated)
	int MaxDashCount = 3;; // 대쉬 수

	UPROPERTY(Replicated)
	float JumpZVelocity = 600.f;; // 점프력

	UPROPERTY(Replicated)
	float AttackDamage = 30.f;; // 프로젝타일 공격력

	UPROPERTY(Replicated)
	float WeaponAttackCoolTime = 0.3f;; // 공격속도

	UPROPERTY(Replicated)
	float CriticalRate = 0.1f;;

	UPROPERTY(Replicated)
	float CriticalDamage = 1.5f;;

	UPROPERTY(Replicated)
	float AbsorbDelay = 0.5f;; // 젤루 흡수 딜레이

	UPROPERTY(Replicated)
	int MaxSavedAmmo = 500;; // 최대 저장 가능한 탄알(젤루)

	UPROPERTY(Replicated)
	int MaxAmmo = 10;; // 최대 재장전 가능한 탈알(젤루)

protected:
	UFUNCTION()
	void OnRep_PlayerColor();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};

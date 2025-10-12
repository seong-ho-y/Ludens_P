// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Components/ActorComponent.h"
#include "Ludens_P/EEnemyColor.h"
#include "Net/UnrealNetwork.h"
#include "LobbyTypes.h"
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
	float MaxHP = 100.f;

	UPROPERTY(Replicated)
	float MaxShield = 100.f;

	UPROPERTY(Replicated)
	float MoveSpeed = 1000.f;

	UPROPERTY(Replicated)
	float ShieldRegenSpeed = 0.5f; // 쉴드 재생 속도
	UPROPERTY(Replicated)
	float DashRechargeTime = 3.f; // 대쉬 재생 속도

	UPROPERTY(Replicated)
	int MaxDashCount = 3; // 대쉬 수

	/*UPROPERTY(Replicated)
	float JumpZVelocity = 600.f; // 점프력*/

	UPROPERTY(Replicated)
	float AttackDamage = 30.f; // 프로젝타일 공격력 ✅
	// ****** ✅

	UPROPERTY(Replicated)
	float WeaponAttackCoolTime = 0.3f; // 공격속도 ✅

	UPROPERTY(Replicated)
	float CriticalRate = 0.1f; // ❌

	UPROPERTY(Replicated)
	float CriticalDamage = 1.5f; // ❌

	UPROPERTY(Replicated)
	float AbsorbDelay = 0.5f; // 젤루 흡수 딜레이 ✅

	UPROPERTY(Replicated)
	int MaxSavedAmmo = 500; // 최대 저장 가능한 탄알(젤루) ✅

	UPROPERTY(Replicated)
	int MaxAmmo = 15; // 최대 재장전 가능한 탈알(젤루) ✅

	/// 로비 관련 수정

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyFieldChanged);

	UPROPERTY(ReplicatedUsing = OnRep_AppearanceId, BlueprintReadOnly, Category = "Lobby")
	int32 AppearanceId = -1;              // 로비에서 고른 외형(A0~A3 등)

	UPROPERTY(ReplicatedUsing = OnRep_PreviewColor, BlueprintReadOnly, Category = "Lobby")
	EEnemyColor PreviewColor = EEnemyColor::Red;   // 폴백(초기값, 편한 값으로)

	UPROPERTY(ReplicatedUsing = OnRep_SelectedColor, BlueprintReadOnly, Category = "Lobby")
	EEnemyColor SelectedColor = EEnemyColor::Red;

	UPROPERTY(ReplicatedUsing = OnRep_SubskillId, BlueprintReadOnly, Category = "Lobby")
	int32 SubskillId = -1;                // 보조 스킬(인덱스)

	// Ready 토글(잠금 여부)
	UPROPERTY(ReplicatedUsing = OnRep_Ready, BlueprintReadOnly, Category = "Lobby")
	bool bReady = false;

	// 로비 선택값 중 하나라도 바뀌면 브로드캐스트(UMG 갱신용)
	UPROPERTY(BlueprintAssignable, Category = "Lobby")
	FOnLobbyFieldChanged OnAnyLobbyFieldChanged;

	UFUNCTION(BlueprintCallable, Category = "Lobby")
	void NotifyAnyLobbyFieldChanged(); // 로비 즉시 반영용 이벤트 트리거

	virtual void CopyProperties(APlayerState* PlayerState) override;
	virtual void OverrideWith(APlayerState* PlayerState) override;
	virtual void SeamlessTravelTo(APlayerState* NewPlayerState) override;

	///



protected:
	UFUNCTION()
	void OnRep_PlayerColor();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	/// 로비 관련 수정

	UFUNCTION() void OnRep_AppearanceId();
	UFUNCTION() void OnRep_PreviewColor();
	UFUNCTION() void OnRep_SelectedColor();
	UFUNCTION() void OnRep_SubskillId();
	UFUNCTION() void OnRep_Ready();



	///
};

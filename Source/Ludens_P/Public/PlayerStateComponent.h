// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Ludens_P/EEnemyColor.h"
#include "Net/UnrealNetwork.h"
#include "Ludens_P/RewardData.h"
#include "PlayerStateComponent.generated.h"

// 플레이어의 상태를 체크하는 컴포넌트
// 쉴드, 체력, 이동속도, 피격 상태, 생존 상태 확인

UENUM(BlueprintType)
enum class ECurrentHPPolicy : uint8 { KeepRatio, KeepCurrent, HealToFull };

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UPlayerStateComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY()
	class ACharacter* Character;
	UPROPERTY()
	class APlayerState_Real* PSR;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Sound)
	TObjectPtr<USoundBase> HitSound;
	
	// Sets default values for this component's properties
	UPlayerStateComponent();

	//플레이어 색
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, ReplicatedUsing = OnRep_PlayerColor, Category = "Player")
	EEnemyColor PlayerColor;
	
	// 플레이어 체력
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category = "Player", Replicated)
	float MaxHP = 50.0f;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Player", Replicated)
	float CurrentHP;

	// 플레이어 쉴드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player", Replicated)
	float MaxShield = 10.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", Replicated)
	float CurrentShield;

	// 플레이어 이동 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", ReplicatedUsing=OnRep_MoveSpeed)
	float MoveSpeed;
private:
	bool bPSRInitialized = false; // PSR 할당 되었는지 확인
	float CalculateMoveSpeed; // 능력 강화시 계산용
	float KnockedMoveSpeed = 100.0f;
	float DeadMoveSpeed = 0.f;
public:
	// 플레이어가 공격 당한 상태인지 확인, 공격 당하면 일정 시간 동안 무적 상태
	UPROPERTY(VisibleAnywhere, Category = "Player", ReplicatedUsing=OnRep_IsAttacked)
	bool IsAttacked = false;
	FTimerHandle InvincibilityTimerHandle;

	//플레이어의 생존 여부
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Player", ReplicatedUsing=OnRep_Dead)
	bool IsDead = false;

	//플레이어의 생존 여부
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Player", ReplicatedUsing=OnRep_Knocked)
	bool IsKnocked = false;

	UPROPERTY(VisibleAnywhere, Category = "Player", Replicated)
	bool bCanRegenShield = false;
	
	void Knocked();
	UFUNCTION(Server, Reliable)
	void Server_Knocked();
	void Dead();
	void TakeDamage(float Amount);
	void EnableShieldRegen();
	void RegenShieldHandle();
protected:
	FTimerHandle bCanRegenShieldTimer;
	FTimerHandle RegenShieldTimer;
public:
	void ResetInvincibility(); // 무적 시간 초기화 함수
	UFUNCTION()
	void OnRep_IsAttacked(); // 피격 당한 상태(무적 시간)일 때 UI 또는 이펙트를 적용하는 함수
	UFUNCTION()
	void OnRep_Dead(); // 죽은 상태일 때 UI 또는 이펙트를 적용하는 함수
	UFUNCTION()
	void OnRep_Knocked();
	UFUNCTION()
	void OnRep_MoveSpeed();

	// 현재치 동반 보정/즉시 반영 계열
	UFUNCTION() void ApplyMaxHP(ERewardOpType Op, float V, ECurrentHPPolicy Policy = ECurrentHPPolicy::KeepRatio);
	UFUNCTION() void ApplyMaxShield(ERewardOpType Op, float V, ECurrentHPPolicy Policy = ECurrentHPPolicy::KeepRatio);
	UFUNCTION() void ApplyShieldRegenSpeed(ERewardOpType Op, float V);

	// PSR→PSC 동기화 편의 함수
	UFUNCTION() void SyncMoveSpeedFromPSR(class APlayerState_Real* PS_R);
	UFUNCTION() void UpdateMoveSpeed(); // CharacterMovement->MaxWalkSpeed 반영
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION()
	void OnRep_PlayerColor();
public:
	UPROPERTY(BlueprintReadOnly)
	FTimerHandle KnockedTimer; // 기절 한 뒤 죽을 때까지 작동하는 타이머
	/*UPROPERTY(BlueprintReadOnly, Replicated)
	float KnockedTimeRemaining; // Knocked 타이머의 남은 시간을 저장 (UI가 사용할 실제 값)
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bIsKnockedTimerPaused; // 타이머가 현재 일시 정지되었는지 여부*/

	
	// UI 업데이트를 위한 델리게이트 선언 (데미지 입는 시점)
	// 이 델리게이트에 블루프린트 UI 함수를 바인딩합니다.
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDamageTakenUI);
    
	UPROPERTY(BlueprintAssignable, Category = "UI")
	FOnDamageTakenUI OnDamageTakenUI; 
    
	// UI 애니메이션 재생을 위한 Multicast 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayDamageUI();
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayKnockedUI();
protected:
	UFUNCTION(Server, Reliable)
	void Server_RequestDamageUI();
	
	// 데미지 비네트 UI의 현재 Opacity 값 (복제 필요 없음)
	UPROPERTY(BlueprintReadOnly, Category = "UI")
	float DamageVignetteOpacity = 0.0f;

	FTimerHandle VignetteTimerHandle;

	void UpdateVignetteOpacity();
public:
	void RevertMoveSpeed();
};

// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TP_WeaponComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Ludens_PCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class URewardSystemComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ALudens_PCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* LookAction;

	// 대쉬 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* DashAction;

	// 근접 공격 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* MeleeAttackAction;

	// 테스트 공격 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* TestAttackAction;

	// 재장전 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* ReloadAction;

	// 무기 공격 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* FireAction;

	// 팀원 소생 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* ReviveAction;

	// 젤루 흡수 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* AbsorbAction;

public:
	ALudens_PCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URewardSystemComponent* RewardSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPlayerAttackComponent* PlayerAttackComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UPlayerStateComponent* PlayerStateComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UTP_WeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UReviveComponent* ReviveComponent;

	UPROPERTY()
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Niagara")
	UNiagaraSystem* DashNiagara;

private:
	UPROPERTY(Replicated) int8 JumpCount = 0; // 점프 횟수
	UPROPERTY(EditAnywhere, Category = "Jump") int8 MaxJumpCount = 2; // 최대 점프 횟수 제한

	// 대쉬 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float DashSpeed = 1500.0f;

protected:
	// 대시 시스템 변수
	UPROPERTY(EditDefaultsOnly, Category = "Dash") int MaxDashCount = 3;
	UPROPERTY(VisibleAnywhere, Category = "Dash", Replicated) int CurrentDashCount = 3;
	UPROPERTY(EditDefaultsOnly, Category = "Dash") float DashCooldown = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash") float DashRechargeTime = 3.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash", Replicated) bool bCanDash = true;

	UPROPERTY(EditDefaultsOnly, Category = "Reload") float ReloadTime = 2.f; // 재장전 시간

	//** 재장전 시스템 변수
	UPROPERTY(EditDefaultsOnly, Category = "Reload") int MaxAmmo; // 최대로 장전 할 수 있는 탄알 수
	UPROPERTY(EditDefaultsOnly, Category = "Reload") int MaxSavedAmmo; // 최대로 저장할 수 있는 탄알 수
	UPROPERTY(EditDefaultsOnly, Category = "Reload", ReplicatedUsing = OnRep_CurrentAmmo) int CurrentAmmo; // 장전 완료 된 탄알

public:
	UPROPERTY(EditDefaultsOnly, Category = "Reload", ReplicatedUsing = OnRep_SavedAmmo) int SavedAmmo; // 저장되어 있는 탄알

private:
	bool bPSRInitialized = false;

protected:
	FTimerHandle DashTimerHandle; // 대시 타이머 핸들

	// 마찰력 원본 값 저장용 변수
	float OriginalGroundFriction = 8.0f;
	float OriginalBrakingDeceleration = 2048.0f;

	FVector2D LastMovementInput; // 마지막 이동 입력 저장

	FTimerHandle DashPhysicsTimerHandle; // 물리 설정 복원 전용
	FTimerHandle DashCooldownTimerHandle; // 대쉬 쿨타임
	FTimerHandle DashRechargeTimerHandle; // 대쉬 차지

	//** 재장전 쿨타임
	FTimerHandle ReloadTimerHandle;
	bool bIsReloading = false; // 재장전하는 중인지 확인

	FTimerHandle AbsorbCompleteTimerHandle;

protected:
	virtual void BeginPlay();
	void Tick(float DeltaTime) override;

	// Jump 함수 선언
	virtual void Jump() override;

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void Landed(const FHitResult& Hit) override; // 땅에 착지 했는지 안 했는지 판단S

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void TestAttack(const FInputActionValue& Value);

	// Dash 함수 선언
	void Dash(const FInputActionValue& Value);

	// 근접 공격인지 부활인지 판단하는 함수 선언
	void Interact(const FInputActionValue& Value);

	// 근접 공격 함수 선언
	void MeleeAttack(const FInputActionValue& Value);

	void Fire(const FInputActionValue& Value);

	void Reload(const FInputActionValue& Value);
	void HandleReload();
	void EndReload();

	void Revive(const FInputActionValue& Value);

	void Absorb(const FInputActionValue& Value);
	void AbsorbComplete(const FInputActionValue& Value);

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	int GetCurrentAmmo() const;

protected:
	UFUNCTION(Category = "Dash") void ResetMovementParams() const; // 마찰력 복원 함수

	UFUNCTION(Server, Reliable) void Server_Jump();
	
	UFUNCTION(Server, Reliable) void Server_Dash();
	UFUNCTION() void RechargeDash(); // 대쉬 충전 함수 선언

	// 무기 공격 함수 선언
	UFUNCTION(Server, Reliable) void Server_Fire(const FInputActionValue& Value);
	
	//** 재장전 함수 선언
	UFUNCTION(Server, Reliable)	void Server_Reload();
	UFUNCTION() void OnRep_SavedAmmo();
	UFUNCTION() void OnRep_CurrentAmmo();

	UFUNCTION(Server, Reliable) void Server_Revive();
	
	UFUNCTION(Server, Reliable) void Server_Absorb(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable) void Server_AbsorbComplete(const FInputActionValue& Value);
	
	UFUNCTION(NetMulticast, Reliable) void MulticastPlayDashEffect();

public:
	//UFUNCTION(BlueprintPure, Category = "Player") APlayerState_Real* GetPSR_FromPSC() const;
};

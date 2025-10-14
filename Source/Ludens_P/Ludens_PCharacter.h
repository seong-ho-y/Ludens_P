// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "TP_WeaponComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
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
class ULudensAppearanceData;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ALudens_PCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;
public:
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	URewardSystemComponent* RewardSystem;

private:
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	// 대쉬 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* DashAction;

	// 근접 공격 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MeleeAttackAction;

	// 테스트 공격 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* TestAttackAction;

	// 재장전 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;

	// 무기 공격 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	// 팀원 소생 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ReviveAction;

	// 젤루 흡수 Input Action
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* AbsorbAction;
	
public:
	UPROPERTY()
	class APlayerState_Real* PSR = nullptr;
private:
	bool bPSRInitialized = false;
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;
	UPROPERTY()
	class UPlayerAttackComponent* PlayerAttackComponent;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UPlayerStateComponent* PlayerStateComponent;
private:
	UPROPERTY()
	class UTP_WeaponComponent* WeaponComponent;
	UPROPERTY()
	class UReviveComponent* ReviveComponent;
	
protected:
	// **1. 나이아가라 컴포넌트 선언**
	// Static Mesh 등 다른 컴포넌트처럼 캐릭터에 부착되어 사용됩니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
	TObjectPtr<UNiagaraComponent> DashNiagaraComponent; // 'DashNiagara' 대신 컴포넌트 사용
	
public:
	ALudens_PCharacter();

protected:
	virtual void BeginPlay();
	void Tick(float DeltaTime) override;

public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	void TestAttack(const FInputActionValue& Value);
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:
	UPROPERTY(Replicated)
	int8 JumpCount = 0; // 점프 횟수
	UPROPERTY(EditAnywhere, Category = "Jump")
	int8 MaxJumpCount = 2; // 최대 점프 횟수 제한

	// 대쉬 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	float DashSpeed = 1500.0f;
	
public:
	void Landed(const FHitResult& Hit) override; // 땅에 착지 했는지 안 했는지 판단

protected:
	UFUNCTION(Server, Reliable)
	void Server_Jump();
	// Jump 함수 선언
	virtual void Jump() override;
	
	UFUNCTION(Server, Reliable)
	void Server_Dash();
	// Dash 함수 선언
	void Dash(const FInputActionValue& Value);
	FTimerHandle DashTimerHandle; // 대시 타이머 핸들
    
	// 마찰력 원본 값 저장용 변수
	float OriginalGroundFriction = 8.0f;
	float OriginalBrakingDeceleration = 2048.0f;

	UFUNCTION(Category="Dash")
	void ResetMovementParams() const; // 마찰력 복원 함수
	FVector2D LastMovementInput; // 마지막 이동 입력 저장

	UFUNCTION()
	void RechargeDash(); // 대쉬 충전 함수 선언
	
	// 대시 시스템 변수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
	int MaxDashCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash", Replicated)
	int CurrentDashCount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
	float DashCooldown = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = "Dash")
	float DashRechargeTime = 3.0f;
	// 1. 쿨타임 시작 시간 복제 변수 추가
	// RepNotify를 사용하여 이 값이 클라이언트에게 전달될 때마다 특정 함수를 호출합니다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing=OnRep_DashCooldownTime, Category = "Dash")
	float ReplicatedDashCooldownStartTime = 0.0f;

	// 2. RepNotify 함수 선언
	UFUNCTION()
	void OnRep_DashCooldownTime();

	
	FTimerHandle DashPhysicsTimerHandle; // 물리 설정 복원 전용
	FTimerHandle DashCooldownTimerHandle; // 대쉬 쿨타임
	FTimerHandle DashRechargeTimerHandle; // 대쉬 차지
	FTimerHandle DashEffectTimerHandle; // **2. 대쉬 이펙트 비활성화 타이머 핸들**
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dash", Replicated)
	bool bCanDash = true;
	
	// 근접 공격인지 부활인지 판단하는 함수 선언
	void Interact(const FInputActionValue& Value);
	
	// 근접 공격 함수 선언
	void MeleeAttack(const FInputActionValue& Value);
	
	// 무기 공격 함수 선언
	UFUNCTION(Server, Reliable)
	void Server_Fire(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);
	
	//** 재장전 함수 선언
	UFUNCTION(Server, Reliable)
	void Server_Reload();
	void Reload(const FInputActionValue& Value);
	void HandleReload();
	void EndReload();
	
	//** 재장전 쿨타임
	FTimerHandle ReloadTimerHandle;
	bool bIsReloading = false; // 재장전하는 중인지 확인
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	float ReloadTime = 2.f; // 재장전 시간
	
	//** 재장전 시스템 변수
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	int MaxSavedAmmo; // 최대로 저장할 수 있는 탄알 수
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reload", ReplicatedUsing = OnRep_SavedAmmo)
	int SavedAmmo; // 저장되어 있는 탄알
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Reload")
	int MaxAmmo; // 최대로 장전 할 수 있는 탄알 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reload", ReplicatedUsing = OnRep_CurrentAmmo)
	int CurrentAmmo; // 장전 완료 된 탄알
	
	UFUNCTION()
	void OnRep_SavedAmmo();
	UFUNCTION()
	void OnRep_CurrentAmmo();
public:
	int GetCurrentAmmo() const;

protected:
	UFUNCTION(Server, Reliable)
	void Server_Revive();
	void Revive(const FInputActionValue& Value);

	void Absorb(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable)
	void Server_Absorb(const FInputActionValue& Value);
	void AbsorbComplete(const FInputActionValue& Value);
	UFUNCTION(Server, Reliable)
	void Server_AbsorbComplete(const FInputActionValue& Value);
	FTimerHandle AbsorbCompleteTimerHandle;
	
public:
	// **3. 컴포넌트 활성화/비활성화용 Multicast 함수**
	UFUNCTION(NetMulticast, Reliable)
	void MulticastControlDashEffect(bool bActivate);
	// **4. 이펙트를 비활성화할 함수**
	void DeactivateDashEffect();
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 외형 적용 추가
	UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
	ULudensAppearanceData* AppearanceDB = nullptr;

	bool bCosmeticsApplied = false;     // 1회 적용 보증
	int32 CachedSubskillId = -1;        // (표현은 안 해도) 선택 스킬 id만 캐시

	virtual void PossessedBy(AController* NewController) override;
	
	UPROPERTY(EditAnywhere, Category = "Tool")
	UActorComponent* ToolComponent;

	void OnInteract();


	// 외형 적용 1회 함수
	UFUNCTION()
	void ApplyCosmeticsFromPSROnce();
	virtual void OnRep_PlayerState() override;   // 클라: PS 세팅 복제 시
};

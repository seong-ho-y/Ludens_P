#pragma once

#include "CoreMinimal.h"
#include "CreatureCombatComponent.h"
#include "EEnemyColor.h"
#include "HitFeedbackComponent.h"
#include "GameFramework/Character.h"
#include "DeathHandlerInterface.h"
#include "StealthInterface.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "EnemyBase.generated.h"

class UBlackboardComponent;
class UEnemyDescriptor;
class UShieldComponent;
class UWidgetComponent;
class UEnemyHealthBarBase;

UCLASS(Abstract)
class LUDENS_P_API AEnemyBase : public ACharacter, public IDeathHandlerInterface, public IStealthInterface
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;
	void RotateToPlayer(float DeltaSeconds);

	virtual void EngageStealth_Implementation() override;
	virtual void DisengageStealth_Implementation() override;
	
	AEnemyBase();

	void Activate(const FVector& Location, const FRotator& Rotation);
	void Deactivate();

	bool IsActive() const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UCreatureCombatComponent* CCC;

	// AActor로부터 상속받은 TakeDamage 함수를 정확한 시그니처로 오버라이드합니다.
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly, Category="Config")
	UEnemyDescriptor* Descriptor = nullptr;

	UFUNCTION(BlueprintCallable, Category="Color")
	void ChangeColorType(EEnemyColor Color);

	virtual void HandleDeath_Implementation() override;

	UFUNCTION()
	void HandleDied();

	UFUNCTION(BlueprintCallable, Category="Material")
	void InitializeMID();
protected:
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION()
	void OnRep_ColorType();
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_ColorType, Category="Color")
	EEnemyColor ColorType;
	
	// 활성화 상태를 저장하고 복제할 변수
	UPROPERTY(ReplicatedUsing = OnRep_IsActive)
	bool bIsActiveInPool;

	// bIsActiveInPool 변수가 클라이언트에서 변경될 때 호출될 함수
	UFUNCTION()
	void OnRep_IsActive();

	// 실제로 활성/비활성 처리를 하는 함수
	void UpdateActiveState(bool bNewIsActive);
	
	
	void EndPlay(EEndPlayReason::Type EndPlayReason);
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* HealthBarWidget;

	UPROPERTY()
	UEnemyHealthBarBase* HealthBarUI;
	// ✨ 위젯의 실제 인스턴스를 저장할 변수
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shield")
	UShieldComponent* ShieldComponent;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VFX")
	UHitFeedbackComponent* HitFeedbackComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Material")
	UMaterialInstanceDynamic* BodyMID;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* SpawnVFX;


	//색상을 설정하는 함수
	void SetBodyColor(EEnemyColor NewColor);

	UFUNCTION()
	void OnHealthUpdated(float NewCurrentHP, float NewMaxHP);
	UFUNCTION()
	void OnShieldsUpdated();

	// ✨ UI를 표시할지 여부를 결정하는 '상태' 변수입니다.
	UPROPERTY(ReplicatedUsing = OnRep_ShouldShowUI)
	bool bShouldShowUI = false;

	// ✨ bShouldShowUI 변수가 복제될 때 클라이언트에서 자동으로 호출될 함수입니다.
	UFUNCTION()
	void OnRep_ShouldShowUI();

	UPROPERTY(Replicated)
	bool bHasTakenDamage = false;

public:
	// 액터가 파괴될 때 호출되는 가상 함수를 오버라이드합니다.
	virtual void Destroyed() override;
protected:
	// 디자이너가 블루프린트에서 쉽게 수정할 수 있도록 스폰 지연 시간을 변수로 만듭니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float SpawnMovementDelay = 2.0f;

	// 지연 시간에 사용할 타이머 핸들
	FTimerHandle SpawnDelayTimerHandle;

	FTimerHandle FinalizeSpawnTimerHandle;

	// 서버에서 타이머가 호출할 함수
	void FinalizeSpawn();

	// 서버와 모든 클라이언트에서 호출될 함수 (실제로 Mesh를 보이게 함)
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_FinalizeSpawn();
	
	// 타이머가 만료되었을 때 호출될 함수
	void ActivateMovementAndAI();

public:
	UFUNCTION(NetMulticast, Reliable)
	void PlayAttackMontage();
	UFUNCTION(NetMulticast, Reliable)
	void PlayDashEffects();
	UFUNCTION(NetMulticast, Reliable)
	void PlayShootMontage();
	UFUNCTION(NetMulticast, Reliable)
	void PlayCastMontage();

protected:
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* MeleeAttackMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* DashMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UNiagaraSystem* DashVFX;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* ShootMontage;
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* CastMontage;
private:
	// ✨ VFX 재생을 모든 클라이언트에게 전파하기 위한 Multicast 함수
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySpawnVFX(FVector_NetQuantize Location, FRotator Rotation);
};
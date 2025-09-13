#pragma once

#include "CoreMinimal.h"
#include "CreatureCombatComponent.h"
#include "EEnemyColor.h"
#include "HitFeedbackComponent.h"
#include "GameFramework/Character.h"
#include "DeathHandlerInterface.h"
#include "EnemyBase.generated.h"

class UEnemyDescriptor;
class UShieldComponent;
class UWidgetComponent;
class UEnemyHealthBarBase;

UCLASS(Abstract)
class LUDENS_P_API AEnemyBase : public ACharacter, public IDeathHandlerInterface
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaTime) override;
	
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

	UPROPERTY()
	UMaterialInstanceDynamic* BodyMID;



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
};
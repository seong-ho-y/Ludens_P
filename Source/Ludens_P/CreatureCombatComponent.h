#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CreatureCombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UCreatureCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCreatureCombatComponent();

	virtual void BeginPlay() override;

	void Attack(AActor* Target); //공격하는 메서드
	void TakeDamage(float Amount); //공격받는 메서드
	void Die(); //죽을 때

	bool IsDead() const { return bIsDead; } //죽었는지 확인

	float GetHealthPercent() const;

protected:
	UPROPERTY(EditAnywhere, Category="Combat")
	float MaxHP = 100.f;

	UPROPERTY(VisibleAnywhere, Category="Combat")
	float CurrentHP;

	UPROPERTY(EditAnywhere, Category="Combat")
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackCooldown = 1.0f;

	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* AttackMontage;

private:
	bool bIsDead = false;
	bool bIsAttacking = false;

	FTimerHandle AttackCooldownHandle;

	void EndAttack();
};

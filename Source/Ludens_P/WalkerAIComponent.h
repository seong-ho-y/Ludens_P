#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WalkerAIComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UWalkerAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWalkerAIComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Attack(AActor* Target); //공격하는 메서드
	
private:
	
	bool bIsAttacking = false;

	FTimerHandle AttackCooldownHandle;
public:
	void StopMovement();              // 현재 AI 이동 멈추기
	void StartChasing(AActor* Target); // 특정 타겟을 향해 이동 시작

	void EndAttack();
	void UpdateAI();
	APawn* FindNearestPlayer();
	void MoveToTarget(APawn* Target);
	// 기본 레퍼런스
	ACharacter* OwnerCharacter = nullptr;
	class UCreatureCombatComponent* Combat = nullptr;

	APawn* CurrentTarget = nullptr;

	UPROPERTY(EditAnywhere, Category="AI")
	float AttackRange = 150.f;

	UPROPERTY(EditAnywhere, Category="AI")
	float SearchInterval = 0.5f;

	float TimeSinceLastSearch = 0.f;


protected:
	UPROPERTY(EditAnywhere, Category="Combat")
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, Category="Combat")
	float AttackCooldown = 1.0f;

	UPROPERTY(EditAnywhere, Category="Combat")
	UAnimMontage* AttackMontage;
	

};

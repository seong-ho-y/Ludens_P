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

private:
	void UpdateAI();
	APawn* FindNearestPlayer();
	void MoveToTarget(APawn* Target);
	void TryAttack(APawn* Target);

	// 기본 레퍼런스
	ACharacter* OwnerCharacter = nullptr;
	class UCreatureCombatComponent* Combat = nullptr;

	APawn* CurrentTarget = nullptr;

	UPROPERTY(EditAnywhere, Category="AI")
	float AttackRange = 150.f;

	UPROPERTY(EditAnywhere, Category="AI")
	float SearchInterval = 0.5f;

	float TimeSinceLastSearch = 0.f;
};

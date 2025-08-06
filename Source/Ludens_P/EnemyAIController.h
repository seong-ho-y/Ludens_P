// EnemyAIController.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

UCLASS()
class  LUDENS_P_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()
	void OnPossess(APawn* InPawn);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	class UBehaviorTree* BehaviorTreeAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	class UBlackboardData* BlackboardAsset;
};

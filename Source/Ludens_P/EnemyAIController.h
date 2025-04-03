// EnemyAIController.h
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogEnemyAI, Log, All);

UCLASS()
class LUDENS_P_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;
	void BeginPlay();
	void UpdateTarget();
	APawn* FindNearestPlayer();
};
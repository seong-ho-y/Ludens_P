// BTT_Fire.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Fire.generated.h"

UCLASS()
class LUDENS_P_API UBTT_Fire : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_Fire();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetActorKey;
};

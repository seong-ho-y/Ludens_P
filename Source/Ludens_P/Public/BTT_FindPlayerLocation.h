// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FindPlayerLocation.generated.h"

/**
 * 
 */
UCLASS()
class LUDENS_P_API UBTT_FindPlayerLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_FindPlayerLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetLocation1Key;
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetLocation2Key;
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetLocation3Key;
};

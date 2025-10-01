// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_CastSpell.generated.h"

/**
 * 
 */
UCLASS()
class LUDENS_P_API UBTT_CastSpell : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTT_CastSpell();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector TargetLocationKey;
	
};

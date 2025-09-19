// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTT_EngageStealth.generated.h"

/**
 * 
 */
UCLASS()
class LUDENS_P_API UBTT_EngageStealth : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTT_EngageStealth();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory) override;
};

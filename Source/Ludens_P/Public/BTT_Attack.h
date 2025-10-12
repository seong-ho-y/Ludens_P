// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Attack.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS()
class LUDENS_P_API UBTT_Attack : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTT_Attack();
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
};

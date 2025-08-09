// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Fire.generated.h"

// BTT_Fire.h
UCLASS()
class LUDENS_P_API UBTT_Fire : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere) FBlackboardKeySelector TargetActorKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

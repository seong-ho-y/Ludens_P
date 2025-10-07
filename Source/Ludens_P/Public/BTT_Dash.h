// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_Dash.generated.h"

/**
 * 
 */
UCLASS()
class LUDENS_P_API UBTT_Dash : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_Dash();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* DashVFX;

	UPROPERTY(EditAnywhere, Category = "Dash")
	FVector DashImpulse = FVector(2000, 0, 500);
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActor;
	
};

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

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory) override;
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp);

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
	float DamageAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
	float AttackRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
	UNiagaraSystem* AttackVFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
	UAnimMontage* AttackAnim;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
	float DamageAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
	float AttackRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
	UNiagaraSystem* AttackVFX;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Blackboard")
	UAnimMontage* AttackAnim;
	
};

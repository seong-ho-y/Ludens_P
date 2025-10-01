// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CastSpell.h"

#include "AIController.h"
#include "MagicComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

class UMagicComponent;

UBTT_CastSpell::UBTT_CastSpell()
{
	NodeName = "Cast Spell";
}

EBTNodeResult::Type UBTT_CastSpell::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn()) return EBTNodeResult::Failed;

	UMagicComponent* MagicComp = AIController->GetPawn()->FindComponentByClass<UMagicComponent>();
	if (!MagicComp) return EBTNodeResult::Failed;

	FVector TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetLocationKey.SelectedKeyName);

	// 위치 값이 유효할 때만 (0,0,0이 아닐 때) 마법 시전
	if (!TargetLocation.IsNearlyZero())
	{
		MagicComp->CastSpellAtLocation(TargetLocation);
	}
	
	return EBTNodeResult::Succeeded;
}
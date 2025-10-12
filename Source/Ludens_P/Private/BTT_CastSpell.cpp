// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_CastSpell.h"

#include "AIController.h"
#include "MagicComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Ludens_P/EnemyBase.h"

class UMagicComponent;

UBTT_CastSpell::UBTT_CastSpell()
{
	NodeName = "Cast Spell";
}

EBTNodeResult::Type UBTT_CastSpell::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* Pawn = AIController ? AIController->GetPawn() : nullptr;
	if (!Pawn) return EBTNodeResult::Failed;

	UMagicComponent* MagicComp = Pawn->FindComponentByClass<UMagicComponent>();
	if (!MagicComp) return EBTNodeResult::Failed;

	FVector TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetLocationKey.SelectedKeyName);

	if (!TargetLocation.IsNearlyZero())
	{
		MagicComp->CastSpellAtLocation(TargetLocation);
		if (AEnemyBase* EnemyChar = Cast<AEnemyBase>(Pawn))
		{
			EnemyChar->PlayCastMontage();
		}
	}
    
	return EBTNodeResult::Succeeded;
}
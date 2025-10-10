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
		// ✨ 1. MagicComponent에 타겟 위치를 알려주며 시전 준비를 시킵니다.
		MagicComp->PrepareToCast(TargetLocation);

		// ✨ 2. 캐릭터에게 마법 시전 몽타주를 재생하라고 명령합니다.
		// (이 예시에서는 캐릭터에 PlayMagicMontage() 함수가 있다고 가정합니다)
		if (AEnemyBase* EnemyChar = Cast<AEnemyBase>(Pawn))
		{
			EnemyChar->PlayCastMontage();
		}
	}
    
	return EBTNodeResult::Succeeded;
}
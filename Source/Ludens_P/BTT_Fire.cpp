// BTT_Fire.cpp
#include "BTT_Fire.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "ShooterCombatComponent.h"

EBTNodeResult::Type UBTT_Fire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8*)
{
	auto* AI = OwnerComp.GetAIOwner();
	auto* Pawn = AI ? AI->GetPawn() : nullptr;
	auto* Combat = Pawn ? Pawn->FindComponentByClass<UShooterCombatComponent>() : nullptr;
	auto* BB = OwnerComp.GetBlackboardComponent();
	auto* Target = BB ? Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName)) : nullptr;

	if (!Combat || !Target) return EBTNodeResult::Failed;

	return Combat->TryFire(Target) ? EBTNodeResult::Succeeded
								   : EBTNodeResult::Failed;
}

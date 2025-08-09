// BTT_Fire.cpp
#include "BTT_Fire.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "ShooterCombatComponent.h"

UBTT_Fire::UBTT_Fire()
{
	NodeName = TEXT("Fire At Target");
}

EBTNodeResult::Type UBTT_Fire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn) return EBTNodeResult::Failed;

	UShooterCombatComponent* CombatComp = AIPawn->FindComponentByClass<UShooterCombatComponent>();
	if (!CombatComp) return EBTNodeResult::Failed;

	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return EBTNodeResult::Failed;

	bool bFired = CombatComp->TryFire(TargetActor);
	return bFired ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
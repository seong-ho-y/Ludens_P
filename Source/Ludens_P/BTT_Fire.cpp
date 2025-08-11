// BTT_Fire.cpp
#include "BTT_Fire.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "ShooterCombatComponent.h"

UBTT_Fire::UBTT_Fire()
{
	NodeName = TEXT("Fire At Target");
}

EBTNodeResult::Type UBTT_Fire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8*)
{
	UE_LOG(LogTemp, Warning, TEXT("BTT_Fire: ExecuteTask ENTER"));
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon){ UE_LOG(LogTemp, Error, TEXT("BTT_Fire: No AIController")); return EBTNodeResult::Failed; }

	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn){ UE_LOG(LogTemp, Error, TEXT("BTT_Fire: No Pawn")); return EBTNodeResult::Failed; }

	UE_LOG(LogTemp, Warning, TEXT("BTT_Fire: Pawn=%s Authority=%d Class=%s"),
		*AIPawn->GetName(), AIPawn->HasAuthority(), *AIPawn->GetClass()->GetName());

	UShooterCombatComponent* CombatComp = AIPawn->FindComponentByClass<UShooterCombatComponent>();
	if (!CombatComp){ UE_LOG(LogTemp, Error, TEXT("BTT_Fire: ShooterCombatComponent NOT FOUND")); return EBTNodeResult::Failed; }

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* Target = BB ? Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName)) : nullptr;
	UE_LOG(LogTemp, Warning, TEXT("BTT_Fire: Target=%s"), Target ? *Target->GetName() : TEXT("None"));
	if (!Target) return EBTNodeResult::Failed;

	const bool bFired = CombatComp->TryFire(Target);
	UE_LOG(LogTemp, Warning, TEXT("BTT_Fire: TryFire -> %d"), bFired);
	return bFired ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}

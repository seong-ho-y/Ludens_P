// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Explode.h"

#include "AIController.h"

EBTNodeResult::Type UBTT_Explode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return EBTNodeResult::Failed;

	if (!MyPawn->HasAuthority()) return EBTNodeResult::Succeeded;

	auto* explodeComp = MyPawn->GetComponentByClass<UExplosionComponent>();
	if (!explodeComp) return EBTNodeResult::Failed;

	explodeComp->Explode();
	return EBTNodeResult::Succeeded;
}

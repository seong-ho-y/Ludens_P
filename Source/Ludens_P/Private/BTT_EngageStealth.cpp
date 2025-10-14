// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_EngageStealth.h"

#include "AIController.h"
#include "StealthInterface.h"

UBTT_EngageStealth::UBTT_EngageStealth()
{
	NodeName = TEXT("Engage Stealth");
}

EBTNodeResult::Type UBTT_EngageStealth::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* MyPawn = AICon->GetPawn();

	if (MyPawn && MyPawn->Implements<UStealthInterface>())
	{
		IStealthInterface::Execute_EngageStealth(MyPawn);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}

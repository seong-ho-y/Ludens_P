// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_DisengageStealth.h"

#include "AIController.h"
#include "StealthInterface.h"

UBTT_DisengageStealth::UBTT_DisengageStealth()
{
	NodeName = TEXT("Disengage Stealth");
}
EBTNodeResult::Type UBTT_DisengageStealth::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	APawn* MyPawn = AICon->GetPawn();
	if (MyPawn && MyPawn->Implements<UStealthInterface>())
	{
		IStealthInterface::Execute_DisengageStealth(MyPawn);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
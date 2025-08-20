// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Laser.h"

#include "AIController.h"
#include "EnemyBase.h"
#include "EnemyCharacter.h"
#include "LaserComponent.h"

UBTT_Laser::UBTT_Laser()
{
	NodeName = TEXT("LaserControl");
	
}

EBTNodeResult::Type UBTT_Laser::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	//유효성 검사
	if (AIController == nullptr) return EBTNodeResult::Failed;
	
	AEnemyBase* Enemy = Cast<AEnemyCharacter>(AIController->GetPawn());
	if (Enemy == nullptr) return EBTNodeResult::Failed;

	ULaserComponent* LaserComp = Enemy->FindComponentByClass<ULaserComponent>();
	if (LaserComp == nullptr) return EBTNodeResult::Failed;
	//--------------------------------유효성 검사 끝 -----------------------------
	
	if (bTurnOn)
	{
		LaserComp->TurnOn();
	}
	else
	{
		LaserComp->TurnOff();
	}
	return EBTNodeResult::Succeeded;
}

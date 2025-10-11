// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_DisengageStealth.h"

#include "AIController.h"
#include "StealthInterface.h"
#include "GameFramework/Character.h"
#include "Ludens_P/EnemyBase.h"

class AEnemyBase;

UBTT_DisengageStealth::UBTT_DisengageStealth()
{
	NodeName = TEXT("Disengage Stealth");
}

EBTNodeResult::Type UBTT_DisengageStealth::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	AEnemyBase* MyEnemy = Cast<AEnemyBase>(AICon->GetPawn());
	if (MyEnemy)
	{
		// ❗ 이제 BTT는 MID를 직접 제어하는 대신, 복제될 변수의 값만 변경합니다.
		// MyEnemy->StealthAmount = 1.0f; // 직접 접근 (변수가 public일 경우)
		MyEnemy->SetStealthAmount(0.9f); // Setter 함수를 만들어서 사용하는 것을 추천

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

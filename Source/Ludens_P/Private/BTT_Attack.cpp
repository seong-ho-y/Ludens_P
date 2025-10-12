// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"

#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Ludens_P/EnemyBase.h"

class AEnemyBase;

UBTT_Attack::UBTT_Attack()
{
	NodeName = TEXT("Attack(Melee)");
	bNotifyTaskFinished = true;
	
}
EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 1. AI 컨트롤러 가져오기
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	// 2. AI가 조종하는 캐릭터(Pawn) 가져오기
	AEnemyBase* Self = Cast<AEnemyBase>(AIController->GetPawn());
	if (!Self)
	{
		return EBTNodeResult::Failed;
	}

	// 3. 캐릭터의 공격 함수 호출 (몽타주 재생 시작)
	Self->PlayAttackMontage();

	// 태스크 성공 반환
	return EBTNodeResult::Succeeded;
}

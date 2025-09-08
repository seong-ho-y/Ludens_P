// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Shoot.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Actor.h"
#include "ShooterCombatComponent.h"

UBTT_Shoot::UBTT_Shoot()
{
	NodeName = TEXT("Shoot (via ShooterCombatComponent)");
	bCreateNodeInstance = false;

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_Shoot, TargetActorKey), AActor::StaticClass());
}
EBTNodeResult::Type UBTT_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	APawn* Pawn = AI ? AI->GetPawn() : nullptr;
	if (!Pawn) return EBTNodeResult::Failed;

	// 컴포넌트 찾기
	auto* Shooter = Pawn->FindComponentByClass<UShooterCombatComponent>();
	if (!Shooter)
	{
		return EBTNodeResult::Failed;
	}

	// 타겟
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AActor* Target = BB ? Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName)) : nullptr;
	if (!IsValid(Target)) return EBTNodeResult::Failed;

	// 발사 시도
	const bool bOk = Shooter->TryFire(Target);
	if (!bOk && bFailIfCannotFireImmediately)
		return EBTNodeResult::Failed;

	return bOk ? EBTNodeResult::Succeeded : EBTNodeResult::InProgress; // 쿨다운/버스트 중엔 재호출될 수 있음
}

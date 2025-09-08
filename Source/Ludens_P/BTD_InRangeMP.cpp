// BTDecorator_InRangeMP.cpp
#include "BTD_InRangeMP.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTD_InRangeMP::UBTD_InRangeMP()
{
	NodeName = TEXT("[MP] Is Target In Range");
}

bool UBTD_InRangeMP::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return false;

	APawn* AIPawn = AICon->GetPawn();
	if (!AIPawn) return false;

	// 멀티플레이 서버 권한 체크
	if (!AIPawn->HasAuthority())
	{
		return false;
	}

	UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(GetSelectedBlackboardKey());
	AActor* TargetActor = Cast<AActor>(TargetObject);
	if (!TargetActor) return false;

	const float Distance = FVector::Dist(TargetActor->GetActorLocation(), AIPawn->GetActorLocation());
	return Distance <= FireRange;
}

#include "BTS_UpdateLaser.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "LaserComponent.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTS_UpdateLaser::UBTS_UpdateLaser()
{
	NodeName = TEXT("Update Laser Target");
	// 얼마나 자주 갱신할지 결정합니다. 0.05초~0.1초가 적당합니다.
	Interval = 0.05f; 
}

void UBTS_UpdateLaser::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 필요한 컴포넌트들을 가져옵니다.
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (AIController == nullptr || BlackboardComp == nullptr) return;

	AEnemyCharacter* EnemyChar = Cast<AEnemyCharacter>(AIController->GetPawn());
	if (EnemyChar == nullptr) return;

	ULaserComponent* LaserComp = EnemyChar->FindComponentByClass<ULaserComponent>();
	if (LaserComp == nullptr) return;

	// 블랙보드에서 현재 타겟을 가져옵니다.
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (TargetActor)
	{
		// ✨ 타겟의 현재 위치로 레이저의 끝 지점을 계속해서 갱신합니다.
		LaserComp->UpdateLaserTarget(TargetActor->GetActorLocation());
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FlyToPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTT_FlyToPlayer::UBTT_FlyToPlayer()
{
	NodeName = TEXT("FlyToPlayer");
	AcceptanceRadius = 500.f;
	
	TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTT_FlyToPlayer, TargetLocationKey));
}

EBTNodeResult::Type UBTT_FlyToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!AICon || !BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
	APawn* Pawn = AICon->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}

	const FVector TargetLocation = BlackboardComp->GetValueAsVector(TargetLocationKey.SelectedKeyName);

	if (TargetLocation == FVector::ZeroVector)
	{
		return EBTNodeResult::Failed;
	}
	const FVector MyLocation = Pawn->GetActorLocation();
	const float DistanceToTarget = FVector::Dist(MyLocation, TargetLocation);
	if (DistanceToTarget <= AcceptanceRadius)
	{
		return EBTNodeResult::Succeeded;
	}

	const FVector DirectionToTarget = (TargetLocation - MyLocation).GetSafeNormal();
	Pawn->AddMovementInput(DirectionToTarget, 1.0f);

	return EBTNodeResult::Succeeded;
}
FString UBTT_FlyToPlayer::GetStaticDescription() const
{
	// 에디터 노드에 어떤 블랙보드 키를 사용하는지 표시해줍니다.
	return FString::Printf(TEXT("Fly To: %s"), *TargetLocationKey.SelectedKeyName.ToString());
}
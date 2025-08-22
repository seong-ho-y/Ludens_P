// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_FindNearestPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Actor.h"

UBTS_FindNearestPlayer::UBTS_FindNearestPlayer()
{
	NodeName = TEXT("FNP_S");
	bNotifyTick = true;
	Interval = 0.5f;
	RandomDeviation = 0.0f;

	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTS_FindNearestPlayer, TargetActorKey), AActor::StaticClass());
}

void UBTS_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	APawn* Self = AI ? AI->GetPawn() : nullptr;
	if (!Self) return;

	AActor* Best = nullptr;
	float BestDistSq = TNumericLimits<float>::Max();

	for (FConstPlayerControllerIterator It = Self->GetWorld()->GetPlayerControllerIterator(); It; ++It)
		if (APawn* P = It->Get()->GetPawn())
		{
			const float D = FVector::DistSquared(P->GetActorLocation(), Self->GetActorLocation());
			if (D < BestDistSq) { BestDistSq = D; Best = P; }
		}

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		BB->SetValueAsObject(TargetActorKey.SelectedKeyName, Best); // 없으면 null로 클리어됨
	}
}
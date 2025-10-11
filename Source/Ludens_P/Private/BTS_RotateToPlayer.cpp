// BTS_RotateToTarget.cpp
#include "BTS_RotateToPlayer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Pawn.h"

UBTS_RotateToPlayer::UBTS_RotateToPlayer()
{
	NodeName = TEXT("Rotate");
	bNotifyTick = true;
	Interval = 0.0f; // 0.0으로 설정하여 매 프레임 실행되도록 합니다.
	RandomDeviation = 0.0f;
}

void UBTS_RotateToPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	APawn* ControllingPawn = AIController ? AIController->GetPawn() : nullptr;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!ControllingPawn || !BlackboardComp) return;

	// 블랙보드에서 타겟 액터를 가져옵니다.
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

	if (TargetActor)
	{
		// 목표 방향으로 부드럽게 회전시키는 로직
		FVector AILocation = ControllingPawn->GetActorLocation();
		FVector TargetLocation = TargetActor->GetActorLocation();

		FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(AILocation, TargetLocation);
		TargetRotation.Pitch = 0.f; // 위아래 회전은 막습니다.
		TargetRotation.Roll = 0.f;

		FRotator NewRotation = FMath::RInterpTo(
			ControllingPawn->GetActorRotation(),
			TargetRotation,
			DeltaSeconds,
			RotationSpeed
		);

		ControllingPawn->SetActorRotation(NewRotation);
	}
}
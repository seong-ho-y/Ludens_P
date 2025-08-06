// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FindNearestPlayer.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

EBTNodeResult::Type UBTT_FindNearestPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    // 기존의 FindNearestPlayer 로직을 여기에 구현
    APawn* NearestPawn = nullptr;
    float MinDistSqr = FLT_MAX;
    TArray<AActor*> AllPlayerPawns;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), AllPlayerPawns);

    for (AActor* Actor : AllPlayerPawns)
    {
        APawn* TestPawn = Cast<APawn>(Actor);
        if (TestPawn && TestPawn->IsPlayerControlled())
        {
            float DistSqr = FVector::DistSquared(TestPawn->GetActorLocation(), AIController->GetPawn()->GetActorLocation());
            if (DistSqr < MinDistSqr)
            {
                MinDistSqr = DistSqr;
                NearestPawn = TestPawn;
            }
        }
    }

    // 찾은 플레이어의 위치를 블랙보드에 저장
    if (NearestPawn)
    {
        UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
        if (BlackboardComp)
        {
            // 찾은 Pawn 객체를 블랙보드에 저장합니다.
            BlackboardComp->SetValueAsObject(TargetActorKey.SelectedKeyName, NearestPawn); //이 메서드가 blackoard에 연결된 키에 저장해줌
            return EBTNodeResult::Succeeded;
        }
    }

    return EBTNodeResult::Failed;
}

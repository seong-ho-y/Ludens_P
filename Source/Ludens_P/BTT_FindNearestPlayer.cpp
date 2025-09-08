#include "BTT_FindNearestPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"

EBTNodeResult::Type UBTT_FindNearestPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AI Controller 가져오기
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    APawn* MyPawn = AIController->GetPawn();
    if (!MyPawn) return EBTNodeResult::Failed;

    // 멀티플레이에서는 서버에서만 로직 수행
    if (!MyPawn->HasAuthority()) return EBTNodeResult::Succeeded; // 서버에서만 타겟 설정

    // 플레이어 Pawn 검색
    APawn* NearestPawn = nullptr;
    float MinDistSqr = FLT_MAX;
    TArray<AActor*> AllPlayerPawns;
    UGameplayStatics::GetAllActorsOfClass(OwnerComp.GetWorld(), APawn::StaticClass(), AllPlayerPawns);

    for (AActor* Actor : AllPlayerPawns)
    {
        APawn* TestPawn = Cast<APawn>(Actor);
        if (TestPawn && TestPawn != MyPawn && TestPawn->IsPlayerControlled())
        {
            float DistSqr = FVector::DistSquared(TestPawn->GetActorLocation(), MyPawn->GetActorLocation());
            if (DistSqr < MinDistSqr)
            {
                MinDistSqr = DistSqr;
                NearestPawn = TestPawn;
            }
        }
    }

    // 찾은 플레이어를 블랙보드에 저장
    if (NearestPawn)
    {
        UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
        if (BlackboardComp)
        {
            BlackboardComp->SetValueAsObject(TargetActorKey.SelectedKeyName, NearestPawn);
            return EBTNodeResult::Succeeded;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("UBTT_FindNearestPlayer: Blackboard component is null"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UBTT_FindNearestPlayer: No player found"));
    }

    return EBTNodeResult::Failed;
}

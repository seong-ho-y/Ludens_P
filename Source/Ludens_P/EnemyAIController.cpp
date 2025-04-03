// EnemyAIController.cpp
#include "EnemyAIController.h"
#include "NavigationSystem.h"
#include "Engine/PawnIterator.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

DEFINE_LOG_CATEGORY(LogEnemyAI);


void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();
	FTimerHandle SearchTimer;
	GetWorldTimerManager().SetTimer(SearchTimer, this, &AEnemyAIController::UpdateTarget, 0.5f, true);
}

void AEnemyAIController::UpdateTarget()
{
	APawn* Target = FindNearestPlayer();
	if (Target)
	{
		MoveToActor(Target);
	}
}
APawn* AEnemyAIController::FindNearestPlayer()
{
	float NearestDistance = FLT_MAX;
	APawn* NearestPawn = nullptr;

	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		APawn* TestPawn = *It;
		if (TestPawn && TestPawn->IsPlayerControlled())
		{
			float Distance = FVector::DistSquared(TestPawn->GetActorLocation(), GetPawn()->GetActorLocation());
			if (Distance < NearestDistance)
			{
				NearestDistance = Distance;
				NearestPawn = TestPawn;
			}
		}
	}

	return NearestPawn;
}

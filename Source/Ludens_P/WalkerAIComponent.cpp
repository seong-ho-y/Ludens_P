#include "WalkerAIComponent.h"
#include "CreatureCombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"

UWalkerAIComponent::UWalkerAIComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UWalkerAIComponent::BeginPlay()
{
    Super::BeginPlay();

    OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (OwnerCharacter)
    {
        Combat = OwnerCharacter->FindComponentByClass<UCreatureCombatComponent>();
    }
}

void UWalkerAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!OwnerCharacter || !Combat || Combat->IsDead()) return;

    TimeSinceLastSearch += DeltaTime;
    if (TimeSinceLastSearch >= SearchInterval)
    {
        UpdateAI();
        TimeSinceLastSearch = 0.f;
    }
}

void UWalkerAIComponent::UpdateAI()
{
    CurrentTarget = FindNearestPlayer();
    if (!CurrentTarget) return;

    float Distance = FVector::Dist(CurrentTarget->GetActorLocation(), OwnerCharacter->GetActorLocation());

    if (Distance <= AttackRange)
    {
        if (AAIController* AI = Cast<AAIController>(OwnerCharacter->GetController()))
        {
            AI->StopMovement();
        }

        Combat->Attack(CurrentTarget);
    }
    else
    {
        MoveToTarget(CurrentTarget);
    }
}

APawn* UWalkerAIComponent::FindNearestPlayer()
{
    APawn* NearestPawn = nullptr;
    float MinDistSqr = FLT_MAX;

    for (TActorIterator<APawn> It(GetWorld()); It; ++It)
    {
        APawn* TestPawn = *It;
        if (TestPawn && TestPawn->IsPlayerControlled())
        {
            float DistSqr = FVector::DistSquared(TestPawn->GetActorLocation(), OwnerCharacter->GetActorLocation());
            if (DistSqr < MinDistSqr)
            {
                MinDistSqr = DistSqr;
                NearestPawn = TestPawn;
            }
        }
    }

    return NearestPawn;
}

void UWalkerAIComponent::MoveToTarget(APawn* Target)
{
    if (!Target) return;

    if (AAIController* AI = Cast<AAIController>(OwnerCharacter->GetController()))
    {
        AI->MoveToActor(Target);
    }
}

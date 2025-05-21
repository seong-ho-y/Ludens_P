#include "WalkerAIComponent.h"
#include "CreatureCombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "EnemyBase.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"

// WalkerAI를 공유하는 Walker형 타입의 적들이 사용하는 소스코드
// 
UWalkerAIComponent::UWalkerAIComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    bAutoActivate = true;
    
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

    float Distance = FVector::Dist(CurrentTarget->GetActorLocation(), OwnerCharacter->GetActorLocation()); //거리계산하기

    if (Distance <= AttackRange) //공격 사거리 판정해서 공격
    {
        if (AAIController* AI = Cast<AAIController>(OwnerCharacter->GetController()))
        {
            AI->StopMovement();
        }

        Attack(CurrentTarget);
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

//공격 로직
void UWalkerAIComponent::Attack(AActor* Target)
{

    if (bIsAttacking || Combat->IsDead() || !Target) return;

    ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
    if (AttackMontage && OwnerChar)
    {
        UE_LOG(LogTemp, Log, TEXT("PlayAnimMontage"));

        OwnerChar->PlayAnimMontage(AttackMontage);
    }

    // 데미지 전달
    if (UCreatureCombatComponent* TargetCombat = Target->FindComponentByClass<UCreatureCombatComponent>()) //상대의 UCreatureCombatComponent를 찾아서 TakeDamage 호출
    {
        TargetCombat->TakeDamage(Damage);
    }

    bIsAttacking = true;
    GetWorld()->GetTimerManager().SetTimer(AttackCooldownHandle, this, &UWalkerAIComponent::EndAttack, AttackCooldown, false);
}

void UWalkerAIComponent::EndAttack()
{
    bIsAttacking = false;
}
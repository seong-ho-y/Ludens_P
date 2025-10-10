// BTT_Dash.cpp

#include "BTT_Dash.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Ludens_P/EnemyBase.h"

UBTT_Dash::UBTT_Dash()
{
	NodeName = TEXT("Dash");
}

EBTNodeResult::Type UBTT_Dash::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	if (AI == nullptr) return EBTNodeResult::Failed;

	// ✨ ACharacter 대신 우리가 만든 캐릭터 클래스로 받습니다.
	AEnemyBase* EnemyChar = Cast<AEnemyBase>(AI->GetCharacter());
	if (!EnemyChar) return EBTNodeResult::Failed;
    
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	AActor* TargetActorPtr = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActor.SelectedKeyName));
	if (!TargetActorPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("No TargetActor :: Dash"));
		return EBTNodeResult::Failed;
	}

	FVector DashDirection = TargetActorPtr->GetActorLocation() - EnemyChar->GetActorLocation();
	DashDirection.Z = 0;
	DashDirection.Normalize();

	// 1. 캐릭터 이동 (자동으로 복제됨)
	const FVector LaunchVelocity = DashDirection * DashImpulse.X + FVector(0, DashImpulse.Y, DashImpulse.Z);
	EnemyChar->LaunchCharacter(LaunchVelocity, true, true);

	// 2. 캐릭터에게 애니메이션과 VFX 재생을 명령
	EnemyChar->PlayDashEffects();
    
	return EBTNodeResult::Succeeded;
}

FString UBTT_Dash::GetStaticDescription() const
{
	return FString::Printf(TEXT("Dash towards Target with impulse: %.1f"), DashImpulse.X);
}
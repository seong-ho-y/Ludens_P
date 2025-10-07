// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Dash.h"

#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

UBTT_Dash::UBTT_Dash()
{
	NodeName = TEXT("Dash");
}
EBTNodeResult::Type UBTT_Dash::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AI = OwnerComp.GetAIOwner();
	if (AI == nullptr) return EBTNodeResult::Failed;
	ACharacter* CC = AI->GetCharacter();
	if (!CC) return EBTNodeResult::Failed;
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	ACharacter* TargetPlayer = Cast<ACharacter>(BlackboardComp->GetValueAsObject(TargetActor.SelectedKeyName));
	if (!TargetPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("No TargetPlayer :: Dash"));
		return EBTNodeResult::Failed;
	}

	FVector DashDirection =	TargetPlayer->GetActorLocation() - CC->GetActorLocation();
	DashDirection.Z = 0;
	DashDirection.Normalize();

	const FVector LaunchVelocity = DashDirection * DashImpulse.X + FVector(0,DashImpulse.Y,DashImpulse.Z);
	CC->LaunchCharacter(LaunchVelocity, true, true);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DashVFX, CC->GetActorLocation());
	return EBTNodeResult::Succeeded;
}
FString UBTT_Dash::GetStaticDescription() const
{
	return FString::Printf(TEXT("Dash towards Target with impulse: %.1f"), DashImpulse.X);
}



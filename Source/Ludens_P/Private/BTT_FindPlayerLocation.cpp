// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FindPlayerLocation.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

UBTT_FindPlayerLocation::UBTT_FindPlayerLocation()
{
	NodeName = "Find Player Locations";
}

EBTNodeResult::Type UBTT_FindPlayerLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return EBTNodeResult::Failed;

	// 월드에 있는 모든 플레이어 캐릭터를 찾습니다.
	TArray<AActor*> PlayerActors;
	// ※ 중요: ACharacter::StaticClass() 부분을 실제 사용하는 플레이어 캐릭터 클래스로 바꿔주세요.
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), PlayerActors);

	if (PlayerActors.Num() == 0)
	{
		// 플레이어가 한 명도 없으면 실패 처리
		return EBTNodeResult::Failed;
	}

	// 찾은 플레이어 수만큼 반복 (최대 3번)
	int32 PlayersFound = PlayerActors.Num();
	if (PlayersFound >= 1)
	{
		BlackboardComp->SetValueAsVector(TargetLocation1Key.SelectedKeyName, PlayerActors[0]->GetActorLocation());
	}
	if (PlayersFound >= 2)
	{
		BlackboardComp->SetValueAsVector(TargetLocation2Key.SelectedKeyName, PlayerActors[1]->GetActorLocation());
	}
	if (PlayersFound >= 3)
	{
		BlackboardComp->SetValueAsVector(TargetLocation3Key.SelectedKeyName, PlayerActors[2]->GetActorLocation());
	}
	
	return EBTNodeResult::Succeeded;
}
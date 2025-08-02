// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FindNearestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class LUDENS_P_API UBTT_FindNearestPlayer : public UBTTaskNode
{
	GENERATED_BODY()

	public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory) override;
	// 얘는 무슨 코드지? -> BT에서 이 BTT를 만났을 때 수행할 행동을 정의

	//블랙보드 킬르 에디터에서 서정할 수 있도록 변수 선언하기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blakcboard")
	FBlackboardKeySelector PlayerLocationKey;
	
};

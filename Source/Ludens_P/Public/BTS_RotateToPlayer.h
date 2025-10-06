// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_RotateToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class LUDENS_P_API UBTS_RotateToPlayer : public UBTService
{
	GENERATED_BODY()
	
public:
        UBTS_RotateToPlayer();
    
protected:
        virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    
        // 타겟 액터를 가져올 블랙보드 키
        UPROPERTY(EditAnywhere, Category = "Blackboard")
        FBlackboardKeySelector TargetActorKey;
    
        // 회전 속도
        UPROPERTY(EditAnywhere, Category = "AI")
        float RotationSpeed = 10.0f;
};

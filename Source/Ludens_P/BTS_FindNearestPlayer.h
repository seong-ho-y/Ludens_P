// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTS_FindNearestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class LUDENS_P_API UBTS_FindNearestPlayer : public UBTService
{
	GENERATED_BODY()
public:
	UBTS_FindNearestPlayer();

protected:
	UPROPERTY(EditAnywhere, Category="Blakcboard")
	FBlackboardKeySelector TargetActorKey;

	UPROPERTY(EditAnywhere, Category="Blakcboard")
	FBlackboardKeySelector TargetLocationKey;

	UPROPERTY(EditAnywhere, Category="Blakcboard")
	FBlackboardKeySelector DistanceToPlayerKey;

	UPROPERTY(EditAnywhere, Category="Blakcboard")
	FBlackboardKeySelector RandNum;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};

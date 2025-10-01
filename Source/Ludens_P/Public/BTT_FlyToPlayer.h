// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_FlyToPlayer.generated.h"

/**
 * 
 */
UCLASS()
class LUDENS_P_API UBTT_FlyToPlayer : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTT_FlyToPlayer();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& Comp, uint8* NodeMemory) override;
	FString GetStaticDescription() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FBlackboardKeySelector TargetLocationKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AcceptanceRadius;
};

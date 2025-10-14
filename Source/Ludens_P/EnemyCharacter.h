// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "EnemyCharacter.generated.h"

/**
 * 
 */

class UBehaviorTree;
class UEnemyDescriptor;

UCLASS()
class LUDENS_P_API AEnemyCharacter : public AEnemyBase
{
	GENERATED_BODY()
public:
	AEnemyCharacter();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree* DefaultBT;
	
	//class UEnemyDescriptor* Descriptor;

	virtual void BeginPlay() override;
	void PossessedBy(AController* NewController) override;
};

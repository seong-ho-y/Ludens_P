#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BlackboardComponent.h" // FBlackboardKeySelector를 위해 추가
#include "BTS_UpdateLaser.generated.h"

UCLASS()
class LUDENS_P_API UBTS_UpdateLaser : public UBTService
{
	GENERATED_BODY()

public:
	UBTS_UpdateLaser();

protected:
	// 이 서비스가 활성화되어 있는 동안 주기적으로 호출될 함수입니다.
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// BT 에디터에서 타겟 액터가 저장된 블랙보드 키를 선택하기 위한 변수입니다.
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;
};
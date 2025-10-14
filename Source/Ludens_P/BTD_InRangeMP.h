// BTDecorator_InRangeMP.h
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_BlackboardBase.h"
#include "BTD_InRangeMP.generated.h"

/**
 * Multiplayer-safe distance check decorator
 */
UCLASS()
class LUDENS_P_API UBTD_InRangeMP : public UBTDecorator_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTD_InRangeMP();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

public:
	/** 사격 범위 */
	UPROPERTY(EditAnywhere, Category="Condition")
	float FireRange = 1000.f;
};

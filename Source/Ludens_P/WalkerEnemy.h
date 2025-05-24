#pragma once

#include "CoreMinimal.h"
#include "EnemyBase.h"
#include "WalkerEnemy.generated.h"

class UWalkerAIComponent;

UCLASS()
class LUDENS_P_API AWalkerEnemy : public AEnemyBase //이렇게 EnemyBase를 상속받는 것을 통해 PoolManager에서 TSubclassOf 형식의 템플릿으로 생성가능
{
	GENERATED_BODY()
    
public:
	AWalkerEnemy();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UWalkerAIComponent* WalkerAI;
	
};

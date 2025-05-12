#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyBase.h"
#include "EnemyPoolManager.generated.h"


UCLASS()
class LUDENS_P_API AEnemyPoolManager : public AActor
{
	GENERATED_BODY()

public:
	AEnemyPoolManager();

	AEnemyBase* GetPooledEnemy(TSubclassOf<AEnemyBase> EnemyClass);
	void ReturnEnemy(AEnemyBase* Enemy);
	void SpawnEnemy();
	void PrepopulatePool(TSubclassOf<AEnemyBase> EnemyClass, int32 Count);

protected:
	virtual void BeginPlay() override;
	void Tick(float DeltaTime);
	TMap<TSubclassOf<AEnemyBase>, TArray<AEnemyBase*>> EnemyPools;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyBase> WalkerEnemyClass;
};

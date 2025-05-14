// EnemyPoolManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBase.h"
#include "EnemyPoolManager.generated.h"

UCLASS()
class LUDENS_P_API AEnemyPoolManager : public AActor
{
	GENERATED_BODY()

public:
	AEnemyPoolManager();

	UFUNCTION(BlueprintCallable)
	AEnemyBase* SpawnEnemy(TSubclassOf<AEnemyBase> EnemyClass, FVector Location, FRotator Rotation);

	void ReturnEnemy(AEnemyBase* Enemy);
	UFUNCTION(BlueprintCallable)
	void AddToPool(AEnemyBase* Enemy);

protected:
	virtual void BeginPlay() override;

private:
	// 여러 종류의 적을 관리할 수 있게 클래스별 배열을 사용
	
	TMap<TSubclassOf<AEnemyBase>, TArray<AEnemyBase*>> EnemyPools;

	AEnemyBase* GetPooledEnemy(TSubclassOf<AEnemyBase> EnemyClass);
};

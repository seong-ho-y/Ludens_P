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
	void LogReplicatedEnemies();
	UFUNCTION(BlueprintCallable)
	AEnemyBase* AddToPool(TSubclassOf<AEnemyBase> EnemyClass, FVector Location, FRotator Rotation);

	//풀링에 사용할 적 클래스
	UPROPERTY(EditAnywhere, Replicated, Category = "Pooling")
	TSubclassOf<AEnemyBase> WalkerClass;
	UPROPERTY(EditAnywhere, Replicated, Category = "Pooling")
	TSubclassOf<AEnemyBase> TankClass;
	UPROPERTY(EditAnywhere, Replicated, Category = "Pooling")
	TSubclassOf<AEnemyBase> StealthClass;
	UPROPERTY(EditAnywhere, Replicated, Category = "Pooling")
	TSubclassOf<AEnemyBase> ShooterClass;


protected:
	virtual void BeginPlay() override;
	
	// 여러 종류의 적을 관리할 수 있게 클래스별 배열을 사용
private:
	TMap<TSubclassOf<AEnemyBase>, TArray<AEnemyBase*>> EnemyPools;

	AEnemyBase* GetPooledEnemy(TSubclassOf<AEnemyBase> EnemyClass);
};

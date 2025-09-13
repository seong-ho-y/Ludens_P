#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyBase.h" // AEnemyBase 포함
#include "EnemyPoolManager.generated.h"

// TArray를 감싸는 구조체를 먼저 정의합니다.
USTRUCT(BlueprintType)
struct FEnemyPool
{
	GENERATED_BODY()

	// *** 바로 이 UPROPERTY() 매크로가 핵심입니다! ***
	// 이 매크로가 없으면 GC가 배열 안의 Enemy들을 모두 제거해버립니다.
	UPROPERTY()
	TArray<AEnemyBase*> PooledEnemies;
};

UCLASS()
class LUDENS_P_API AEnemyPoolManager : public AActor
{
	GENERATED_BODY()

public:
	AEnemyPoolManager();

	// 지정된 위치에 적을 스폰시키는 메인 함수 (서버에서만 호출되어야 함)
	AEnemyBase* SpawnEnemy(TSubclassOf<AEnemyBase> EnemyClass, const FVector& Location, const FRotator& Rotation,EEnemyColor EnemyColor);

	// 적을 풀로 되돌리는 함수 (서버에서만 호출되어야 함)
	void ReturnEnemy(AEnemyBase* Enemy);

protected:
	virtual void BeginPlay() override;

	// 각 Enemy 클래스별로 풀을 관리 (서버에만 존재, 복제 불필요)
	UPROPERTY()
	TMap<TSubclassOf<AEnemyBase>, FEnemyPool> EnemyPools;

public:
	// 미리 생성해둘 Enemy 클래스들
	UPROPERTY(EditAnywhere, Category = "Pool Config")
	TSubclassOf<AEnemyBase> ShooterClass;
	UPROPERTY(EditAnywhere, Category="Pool Config")
	TSubclassOf<AEnemyBase> WalkerClass;
	UPROPERTY(EditAnywhere, Category="Pool Config")
	TSubclassOf<AEnemyBase> TankerClass;
	UPROPERTY(EditAnywhere, Category="Pool Config")
	TSubclassOf<AEnemyBase> ChargerClass;
	UPROPERTY(EditAnywhere, Category="Pool Config")
	TSubclassOf<AEnemyBase> RunnerClass;
	UPROPERTY(EditAnywhere, Category="Pool Config")
	TSubclassOf<AEnemyBase> SniperClass;
	UPROPERTY(EditAnywhere, Category="Pool Config")
    TSubclassOf<AEnemyBase> ExploClass;
	UPROPERTY(EditAnywhere, Category="Pool Config")
	TSubclassOf<AEnemyBase> SupportClass;
	UPROPERTY(EditAnywhere, Category="Pool Config")
	TSubclassOf<AEnemyBase> MagicClass;
	
	UPROPERTY(EditAnywhere, Category="Pool Config")
	int32 PoolSize = 10;
	
	// ... 다른 Enemy 타입들도 추가 ...
};
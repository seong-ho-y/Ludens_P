#include "EnemyPoolManager.h"

AEnemyPoolManager::AEnemyPoolManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyPoolManager::BeginPlay()
{
	Super::BeginPlay();

	// 예시: 초기 3종류 적을 각각 10개씩 미리 생성
	//PrepopulatePool(WalkerClass, 10);
	//PrepopulatePool(ShooterClass, 5);
	//PrepopulatePool(SupporterClass, 3);
}

void AEnemyPoolManager::PrepopulatePool(TSubclassOf<AEnemyBase> EnemyClass, int32 Count)
{
	UWorld* World = GetWorld();
	if (!World || !EnemyClass) return;

	if (!EnemyPools.Contains(EnemyClass))
	{
		EnemyPools.Add(EnemyClass, TArray<AEnemyBase*>());
	}

	for (int32 i = 0; i < Count; ++i)
	{
		AEnemyBase* Enemy = World->SpawnActor<AEnemyBase>(EnemyClass);
		Enemy->SetActive(false);
		EnemyPools[EnemyClass].Add(Enemy);
	}
}

AEnemyBase* AEnemyPoolManager::GetPooledEnemy(TSubclassOf<AEnemyBase> EnemyClass)
{
	if (!EnemyPools.Contains(EnemyClass))
	{
		EnemyPools.Add(EnemyClass, TArray<AEnemyBase*>());
	}

	for (AEnemyBase* Enemy : EnemyPools[EnemyClass])
	{
		if (!Enemy->IsActive())
		{
			return Enemy;
		}
	}

	// 부족하면 자동 확장
	AEnemyBase* NewEnemy = GetWorld()->SpawnActor<AEnemyBase>(EnemyClass);
	NewEnemy->SetActive(false);
	EnemyPools[EnemyClass].Add(NewEnemy);
	return NewEnemy;
}

void AEnemyPoolManager::ReturnEnemy(AEnemyBase* Enemy)
{
	if (Enemy)
	{
		Enemy->SetActive(false);
	}
}

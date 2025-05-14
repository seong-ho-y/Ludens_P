// EnemyPoolManager.cpp
#include "EnemyPoolManager.h"
#include "Kismet/GameplayStatics.h"

AEnemyPoolManager::AEnemyPoolManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEnemyPoolManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyPoolManager::AddToPool(AEnemyBase* Enemy)
{
	if (!IsValid(Enemy))
	{
		UE_LOG(LogTemp, Warning, TEXT("AddToPool: Enemy is invalid"));
		return;
	}

	TSubclassOf<AEnemyBase> EnemyClass = Enemy->GetClass();

	EnemyPools.FindOrAdd(EnemyClass).Add(Enemy);
	Enemy->SetActive(false);

	UE_LOG(LogTemp, Log, TEXT("✅ %s added to pool."), *Enemy->GetName());
}

AEnemyBase* AEnemyPoolManager::GetPooledEnemy(TSubclassOf<AEnemyBase> EnemyClass)
{
	if (!EnemyPools.Contains(EnemyClass))
	{
		EnemyPools.Add(EnemyClass, TArray<AEnemyBase*>());
	}

	for (AEnemyBase* Enemy : EnemyPools[EnemyClass])
	{
		if (!Enemy->IsActive()) // SetActive 함수로 관리 중이라면
		{
			return Enemy;
		}
	}

	return nullptr;
}

AEnemyBase* AEnemyPoolManager::SpawnEnemy(TSubclassOf<AEnemyBase> EnemyClass, FVector Location, FRotator Rotation)
{
	AEnemyBase* Enemy = GetPooledEnemy(EnemyClass);

	if (Enemy == nullptr)
	{
		Enemy = GetWorld()->SpawnActor<AEnemyBase>(EnemyClass, Location, Rotation);
		if (Enemy)
		{
			EnemyPools[EnemyClass].Add(Enemy);
		}
	}

	if (Enemy)
	{
		Enemy->SetActorLocation(Location);
		Enemy->SetActorRotation(Rotation);
		Enemy->SetActive(true);
	}

	return Enemy;
}

void AEnemyPoolManager::ReturnEnemy(AEnemyBase* Enemy)
{
	if (Enemy)
	{
		Enemy->SetActive(false);
	}
}

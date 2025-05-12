#include "EnemyPoolManager.h"

#include "WalkerEnemy.h"

AEnemyPoolManager::AEnemyPoolManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

// PoolManager는 적을 많이 소환해야될 때(객체) 미리 생성해놓고 가져다가 쓰는 로직

void AEnemyPoolManager::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		EnableInput(PC);
	}

	if (HasAuthority()) // ✅ 서버에서만 실행
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyPoolManager BeginPlay (SERVER)"));
		PrepopulatePool(WalkerEnemyClass, 10);

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyPoolManager BeginPlay (CLIENT)"));
	}
}
void AEnemyPoolManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


// 처음 PoolManager에서 초기화
void AEnemyPoolManager::PrepopulatePool(TSubclassOf<AEnemyBase> EnemyClass, int32 Count) //소환할 EnemyClass랑 개수
{
	UE_LOG(LogTemp, Log, TEXT("EnemyPool"));
	UWorld* World = GetWorld(); //World 가져오기
	if (!World || !EnemyClass) return;

	if (!EnemyPools.Contains(EnemyClass))
	{
		EnemyPools.Add(EnemyClass, TArray<AEnemyBase*>());
	}

	for (int32 i = 0; i < Count; ++i)
	{
		AEnemyBase* Enemy = World->SpawnActor<AEnemyBase>(EnemyClass);
		if (!Enemy)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to spawn Enemy [%d]"), i);
			continue;
		}

		Enemy->SetActive(false);
		EnemyPools[EnemyClass].Add(Enemy);

		UE_LOG(LogTemp, Log, TEXT("Enemy [%d] successfully spawned"), i);
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
void AEnemyPoolManager::SpawnEnemy()
{
	UE_LOG(LogTemp, Warning, TEXT("SpawnEnemy Called"));
	AEnemyBase* Enemy = GetPooledEnemy(WalkerEnemyClass);
	if (Enemy)
	{
		FVector SpawnLocation = FVector(1884.44f, 1630.45f, -20.0f);
		Enemy->SetActorLocation(SpawnLocation);
		Enemy->SetActive(true);

		UE_LOG(LogTemp, Warning, TEXT("Enemy Spawned via input!"));
	}
}


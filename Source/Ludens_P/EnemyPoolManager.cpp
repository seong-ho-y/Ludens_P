// EnemyPoolManager.cpp
#include "EnemyPoolManager.h"

#include "WalkerEnemy.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AEnemyPoolManager::AEnemyPoolManager()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicateMovement(false); // PoolManager는 고정 위치니까

}

void AEnemyPoolManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Client WalkerClass is %s"),
	   WalkerClass ? *WalkerClass->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("👁️ PoolManager BeginPlay on: %s | Role: %d"),
		HasAuthority() ? TEXT("Server") : TEXT("Client"),
		(int32)GetLocalRole());
	
	for (int i = 0; i < 3; ++i)
	{
		if (HasAuthority())
		{
			AddToPool(WalkerClass, FVector(300, 300, 300), FRotator::ZeroRotator);
			AddToPool(TankClass, FVector(300, 300, 300), FRotator::ZeroRotator);
		}
	}
	if (!HasAuthority())
	{
		FTimerHandle DelayHandle;
		GetWorldTimerManager().SetTimer(DelayHandle, this, &AEnemyPoolManager::LogReplicatedEnemies, 1.0f, false);
	}
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), FoundEnemies);
	
}

AEnemyBase* AEnemyPoolManager::AddToPool(TSubclassOf<AEnemyBase> EnemyClass, FVector Location, FRotator Rotation)
{
	if (!HasAuthority() || !EnemyClass) return nullptr;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AEnemyBase* Enemy = GetWorld()->SpawnActor<AEnemyBase>(EnemyClass, Location, Rotation, Params);
	if (!Enemy)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ SpawnActor failed for class: %s"), *EnemyClass->GetName());
		return nullptr;
	}

	// ✅ 여기서만 풀에 추가
	EnemyPools.FindOrAdd(EnemyClass).Add(Enemy);
	UE_LOG(LogTemp, Log, TEXT("✅ Enemy spawned and added to pool: %s"), *Enemy->GetName());

	// ✅ 복제 완료 후에 SetActive(false) 처리 (복제 타이밍 보장용)
	FTimerHandle DelayHandle;
	GetWorldTimerManager().SetTimer(DelayHandle, [Enemy]()
	{
		if (IsValid(Enemy))
		{
			Enemy->SetActive(false);
			UE_LOG(LogTemp, Warning, TEXT("🕒 SetActive(false) 완료: %s"), *Enemy->GetName());
		}
	}, 3.0f, false); // 기존처럼 3초 딜레이 고정

	return Enemy;
}


AEnemyBase* AEnemyPoolManager::GetPooledEnemy(TSubclassOf<AEnemyBase> EnemyClass)
{
	if (!EnemyPools.Contains(EnemyClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("📦 Pool not found for class: %s → creating new pool"), *EnemyClass->GetName());
		EnemyPools.Add(EnemyClass, TArray<AEnemyBase*>());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("📦 Found pool for class: %s → %d enemies in pool"), 
			*EnemyClass->GetName(), EnemyPools[EnemyClass].Num());
	}

	for (AEnemyBase* Enemy : EnemyPools[EnemyClass])
	{
		if (!IsValid(Enemy))
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Invalid enemy pointer in pool for class: %s"), *EnemyClass->GetName());
			continue;
		}

		if (!Enemy->IsActive())
		{
			UE_LOG(LogTemp, Warning, TEXT("🔄 Reusing inactive enemy: %s"), *Enemy->GetName());
			Enemy->MulticastSetActive(true);
			return Enemy;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("🟡 Skipping active enemy: %s"), *Enemy->GetName());
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("🚫 No reusable enemies available in pool for class: %s"), *EnemyClass->GetName());
	return nullptr;
}


AEnemyBase* AEnemyPoolManager::SpawnEnemy(TSubclassOf<AEnemyBase> EnemyClass, FVector Location, FRotator Rotation)
{
	AEnemyBase* Enemy = GetPooledEnemy(EnemyClass);

	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("📌 No pooled enemy available. Spawning new one."));
		Enemy = AddToPool(EnemyClass, Location, Rotation);

		// AddToPool()에서 자동으로 SetActive(false) 되었으므로,
		// SetActive(true)로 바꾸려면 여기서 명시적으로
		if (Enemy)
		{
			Enemy->MulticastSetActive(true);
		}
	}

	if (Enemy)
	{
		Enemy->SetActorLocation(Location);
		Enemy->SetActorRotation(Rotation);
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
void AEnemyPoolManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
	DOREPLIFETIME(AEnemyPoolManager, WalkerClass);
}
void AEnemyPoolManager::LogReplicatedEnemies()
{
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), FoundEnemies);

	UE_LOG(LogTemp, Warning, TEXT("⏱️ [DELAYED] Found %d replicated enemies"), FoundEnemies.Num());
	for (AActor* Actor : FoundEnemies)
	{
		UE_LOG(LogTemp, Warning, TEXT("🧩 [Client] Replicated Enemy: %s at %s"),
			*Actor->GetName(),
			*Actor->GetActorLocation().ToString());
	}
}

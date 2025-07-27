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
	/*UE_LOG(LogTemp, Warning, TEXT("Client WalkerClass is %s"),
	   WalkerClass ? *WalkerClass->GetName() : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("👁️ PoolManager BeginPlay on: %s | Role: %d"),
		HasAuthority() ? TEXT("Server") : TEXT("Client"),
		(int32)GetLocalRole());
		*/
	
	for (int i = 0; i < 10; ++i)
	{
		if (HasAuthority())
		{
			//AddToPool(WalkerClass, FVector(300, 300, 300), FRotator::ZeroRotator);
			//AddToPool(TankClass, FVector(300, 300, 300), FRotator::ZeroRotator);
			//AddToPool(StealthClass, FVector(300, 300, 300), FRotator::ZeroRotator);
			AddToPool(ShooterClass, FVector(300, 300, 300), FRotator::ZeroRotator);
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
		return nullptr;
	}

	// 풀에 추가합니다.
	EnemyPools.FindOrAdd(EnemyClass).Add(Enemy);

	// 생성 직후 즉시 비활성화 상태로 만듭니다.
	// 이 상태는 클라이언트에 복제되며, SpawnEnemy에서 필요할 때 활성화됩니다.
	Enemy->SetActive(false);

	return Enemy;
}


AEnemyBase* AEnemyPoolManager::GetPooledEnemy(TSubclassOf<AEnemyBase> EnemyClass, EEnemyColor DesiredColor)
{
	if (!EnemyPools.Contains(EnemyClass))
	{
		EnemyPools.Add(EnemyClass, TArray<AEnemyBase*>());
	}

	for (AEnemyBase* Enemy : EnemyPools[EnemyClass])
	{
		if (!IsValid(Enemy))
		{
			continue;
		}

		// 비활성화 상태이고, 원하는 색상과 일치하는 적을 찾습니다.
		if (!Enemy->IsActive() && Enemy->GetEnemyColor() == DesiredColor)
		{
			Enemy->MulticastSetActive(true);
			return Enemy;
		}
	}

	// 만약 원하는 색상의 비활성 적이 없다면, 그냥 아무 비활성 적이나 찾아서 색상을 변경해 사용합니다.
	for (AEnemyBase* Enemy : EnemyPools[EnemyClass])
	{
		if (IsValid(Enemy) && !Enemy->IsActive())
		{
			Enemy->SetupEnemyForColor(DesiredColor); // 여기서 색상을 변경합니다.
			Enemy->MulticastSetActive(true);
			return Enemy;
		}
	}

	return nullptr; // 재사용할 적이 전혀 없음
}


AEnemyBase* AEnemyPoolManager::SpawnEnemy(TSubclassOf<AEnemyBase> EnemyClass, FVector Location, FRotator Rotation, EEnemyColor EnemyColor)
{
	AEnemyBase* Enemy = GetPooledEnemy(EnemyClass, EnemyColor);

	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("📌 No pooled enemy available. Spawning new one."));
		Enemy = AddToPool(EnemyClass, Location, Rotation);

		if (Enemy)
		{
			Enemy->SetupEnemyForColor(EnemyColor);
		}
	}

	if (Enemy)
	{
		Enemy->SetActorLocation(Location);
		Enemy->SetActorRotation(Rotation);
		Enemy->MulticastSetActive(true);
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
void AEnemyPoolManager::LogReplicatedEnemies() //적이 잘 복제됐는지 확인
{
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyBase::StaticClass(), FoundEnemies);

	//UE_LOG(LogTemp, Warning, TEXT("⏱️ [DELAYED] Found %d replicated enemies"), FoundEnemies.Num());
	for (AActor* Actor : FoundEnemies)
	{
		//UE_LOG(LogTemp, Warning, TEXT("🧩 [Client] Replicated Enemy: %s at %s"),
		//	*Actor->GetName(),
		//	*Actor->GetActorLocation().ToString());
	}
}

#include "EnemyPoolManager.h"

#include "GenericPlatform/GenericPlatformCrashContext.h"

AEnemyPoolManager::AEnemyPoolManager()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true; // 매니저 자체는 월드에 존재해야 하므로 복제
}

// 게임 시작 시 풀을 미리 생성 (서버에서만 실행)
void AEnemyPoolManager::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        // WalkerClass를 미리 생성해서 풀에 넣어둠
        if (WalkerClass && TankerClass && RunnerClass && SniperClass)
        {
            for (int32 i = 0; i < PoolSize; ++i)
            {
                FActorSpawnParameters Params;
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                FVector SpawnLoc = FVector(300.f, 300.f, 300.f);
                FRotator SpawnRot = FRotator::ZeroRotator;
                AEnemyBase* NewEnemy = GetWorld()->SpawnActor<AEnemyBase>(WalkerClass, SpawnLoc, SpawnRot, Params);
                AEnemyBase* NewEnemy_T = GetWorld()->SpawnActor<AEnemyBase>(TankerClass, SpawnLoc, SpawnRot, Params);
                AEnemyBase* NewEnemy_R = GetWorld()->SpawnActor<AEnemyBase>(RunnerClass, SpawnLoc, SpawnRot, Params);
                AEnemyBase* NewEnemy_S = GetWorld()->SpawnActor<AEnemyBase>(SniperClass, SpawnLoc, SpawnRot, Params);
                if (NewEnemy && NewEnemy_T)
                {
                    EnemyPools.FindOrAdd(WalkerClass).PooledEnemies.Add(NewEnemy);
                    EnemyPools.FindOrAdd(TankerClass).PooledEnemies.Add(NewEnemy_T);
                    EnemyPools.FindOrAdd(RunnerClass).PooledEnemies.Add(NewEnemy_R);
                    EnemyPools.FindOrAdd(SniperClass).PooledEnemies.Add(NewEnemy_S);
                    NewEnemy->Deactivate(); // 생성 직후 바로 비활성화
                    NewEnemy_T->Deactivate();
                    NewEnemy_R->Deactivate();
                    NewEnemy_S->Deactivate();
                }
            }
        }
        // ... 다른 Enemy 타입 풀 생성 ...
    }
}

// 적 스폰 요청 처리 (서버에서만 실행)
AEnemyBase* AEnemyPoolManager::SpawnEnemy(TSubclassOf<AEnemyBase> EnemyClass, const FVector& Location, const FRotator& Rotation, EEnemyColor EnemyColor)
{
    if (!HasAuthority() || !EnemyClass) return nullptr;

    // 해당 클래스의 풀이 있는지 확인
    FEnemyPool* Pool = EnemyPools.Find(EnemyClass);
    if (Pool)
    {
        for (AEnemyBase* Enemy : Pool->PooledEnemies)
        {
            if (Enemy)
            {
                if (!Enemy->IsActive())
                {
                    Enemy->Activate(Location, Rotation);
                    Enemy->ChangeColorType(EnemyColor);
                    UE_LOG(LogTemp, Log, TEXT("spawned via pool!!!"));
                    return Enemy;
                }
            }
            else {UE_LOG(LogTemp, Warning, TEXT("No Enemy"));}
        }
    }

    // 풀에 사용 가능한 적이 없으면 새로 생성
    // (실제 게임에서는 풀 크기를 초과하지 않도록 제한하는 것이 좋음)
    UE_LOG(LogTemp, Warning, TEXT("No available enemy in pool for %s. Spawning a new one."), *EnemyClass->GetName());
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    AEnemyBase* NewEnemy = GetWorld()->SpawnActor<AEnemyBase>(EnemyClass, Location, Rotation, Params);
    if (NewEnemy)
    {
        EnemyPools.FindOrAdd(EnemyClass).PooledEnemies.Add(NewEnemy);
        // 새로 생성된 적은 기본적으로 활성화 상태로 시작
        NewEnemy->Activate(Location, Rotation);
    }
    return NewEnemy;
}

// 적을 풀로 반납 (서버에서만 실행)
void AEnemyPoolManager::ReturnEnemy(AEnemyBase* Enemy)
{
    if (!HasAuthority() || !Enemy) return;

    Enemy->Deactivate();
}
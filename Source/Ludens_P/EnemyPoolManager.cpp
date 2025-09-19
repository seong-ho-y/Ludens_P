#include "EnemyPoolManager.h"


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
        if (TankerClass && RunnerClass && SniperClass)
        {
            for (int32 i = 0; i < PoolSize; ++i)
            {
                FActorSpawnParameters Params;
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
                FVector SpawnLoc = FVector(300.f, 300.f, 300.f);
                FRotator SpawnRot = FRotator::ZeroRotator;
                AEnemyBase* NewEnemy_S = GetWorld()->SpawnActor<AEnemyBase>(ShooterClass, SpawnLoc, SpawnRot, Params);
                //AEnemyBase* NewEnemy = GetWorld()->SpawnActor<AEnemyBase>(WalkerClass, SpawnLoc, SpawnRot, Params);
                AEnemyBase* NewEnemy_T = GetWorld()->SpawnActor<AEnemyBase>(TankerClass, SpawnLoc, SpawnRot, Params);
                AEnemyBase* NewEnemy_R = GetWorld()->SpawnActor<AEnemyBase>(RunnerClass, SpawnLoc, SpawnRot, Params);
                AEnemyBase* NewEnemy_Sp = GetWorld()->SpawnActor<AEnemyBase>(SniperClass, SpawnLoc, SpawnRot, Params);
                AEnemyBase* NewEnemy_Ex = GetWorld()->SpawnActor<AEnemyBase>(ExploClass, SpawnLoc, SpawnRot, Params);
                {
                    EnemyPools.FindOrAdd(ShooterClass).PooledEnemies.Add(NewEnemy_S);
                    //EnemyPools.FindOrAdd(WalkerClass).PooledEnemies.Add(NewEnemy);
                    EnemyPools.FindOrAdd(TankerClass).PooledEnemies.Add(NewEnemy_T);
                    EnemyPools.FindOrAdd(RunnerClass).PooledEnemies.Add(NewEnemy_R);
                    EnemyPools.FindOrAdd(SniperClass).PooledEnemies.Add(NewEnemy_Sp);
                    EnemyPools.FindOrAdd(ExploClass).PooledEnemies.Add(NewEnemy_Ex);
                    //NewEnemy->Deactivate(); // 생성 직후 바로 비활성화
                    NewEnemy_S->Deactivate();
                    NewEnemy_T->Deactivate();
                    NewEnemy_R->Deactivate();
                    NewEnemy_Sp->Deactivate();
                    NewEnemy_Ex->Deactivate();
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

    // 해당 클래스의 풀이 있는지 확인하고, 없으면 새로 생성
    FEnemyPool& Pool = EnemyPools.FindOrAdd(EnemyClass);

    // 1. 풀에서 비활성화된 적을 먼저 찾아본다.
    for (AEnemyBase* Enemy : Pool.PooledEnemies)
    {
        // 유효하고, 비활성화 상태인 적을 찾았다면 재사용
        if (Enemy && !Enemy->IsActive())
        {
            Enemy->Activate(Location, Rotation);
            Enemy->ChangeColorType(EnemyColor);
            UE_LOG(LogTemp, Log, TEXT("Reused an enemy from the pool!"));
            return Enemy; // 찾았으니 즉시 반환
        }
    }

    // 2. 여기까지 왔다는 것은 풀에 재사용할 적이 없다는 의미.
    // 따라서 새로운 적을 생성한다.
    UE_LOG(LogTemp, Warning, TEXT("No available enemy in pool for %s. Spawning a new one."), *EnemyClass->GetName());

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AEnemyBase* NewEnemy = GetWorld()->SpawnActor<AEnemyBase>(EnemyClass, Location, Rotation, Params);

    if (NewEnemy)
    {
        Pool.PooledEnemies.Add(NewEnemy); // 새로 생성했으니 풀에 추가
        NewEnemy->Activate(Location, Rotation); // Activate 함수가 있다면 호출 (없어도 무방)
        NewEnemy->ChangeColorType(EnemyColor);
    }

    return NewEnemy; // 새로 생성한 적을 반환
}
     

// 적을 풀로 반납 (서버에서만 실행)
void AEnemyPoolManager::ReturnEnemy(AEnemyBase* Enemy) const
{
    if (!HasAuthority() || !Enemy) return;

    Enemy->Deactivate();
}

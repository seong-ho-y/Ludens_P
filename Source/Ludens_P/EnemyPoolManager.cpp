#include "EnemyPoolManager.h"

#include "Ludens_PGameMode.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


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
                AEnemyBase* NewEnemy_St = GetWorld()->SpawnActor<AEnemyBase>(ChargerClass, SpawnLoc, SpawnRot, Params);
                AEnemyBase* NewEnemy_M = GetWorld()->SpawnActor<AEnemyBase>(MagicClass, SpawnLoc, SpawnRot, Params);
                {
                    EnemyPools.FindOrAdd(ShooterClass).PooledEnemies.Add(NewEnemy_S);
                    //EnemyPools.FindOrAdd(WalkerClass).PooledEnemies.Add(NewEnemy);
                    EnemyPools.FindOrAdd(TankerClass).PooledEnemies.Add(NewEnemy_T);
                    EnemyPools.FindOrAdd(RunnerClass).PooledEnemies.Add(NewEnemy_R);
                    EnemyPools.FindOrAdd(SniperClass).PooledEnemies.Add(NewEnemy_Sp);
                    EnemyPools.FindOrAdd(ExploClass).PooledEnemies.Add(NewEnemy_Ex);
                    EnemyPools.FindOrAdd(ChargerClass).PooledEnemies.Add(NewEnemy_St);
                    EnemyPools.FindOrAdd(MagicClass).PooledEnemies.Add(NewEnemy_M);
                    //NewEnemy->Deactivate(); // 생성 직후 바로 비활성화
                    NewEnemy_S->Deactivate();
                    NewEnemy_T->Deactivate();
                    NewEnemy_R->Deactivate();
                    NewEnemy_Sp->Deactivate();
                    NewEnemy_Ex->Deactivate();
                    NewEnemy_St->Deactivate();
                    NewEnemy_M->Deactivate();
                }
            }
        }
        // ... 다른 Enemy 타입 풀 생성 ...
    }
}



// 적 스폰 요청 처리 (서버에서만 실행)
AEnemyBase* AEnemyPoolManager:: SpawnEnemy(const FEnemySpawnProfile& Profile, const FVector& Location, const FRotator& Rotation)
{
    if (!HasAuthority() || !Profile.EnemyClass)
    {
        return nullptr;
    }

    // 해당 클래스의 풀을 찾거나 새로 만듭니다.
    FEnemyPool& Pool = EnemyPools.FindOrAdd(Profile.EnemyClass);


    // 2. 풀에서 재사용 가능한 적을 먼저 찾습니다.
    for (AEnemyBase* Enemy : Pool.PooledEnemies)
    {
        // 유효하고, 비활성화된(숨겨진) 적을 찾았다면 재사용합니다.
        if (Enemy && Enemy->IsHidden()) // IsHidden()은 액터가 숨겨져 있는지 확인하는 표준 함수입니다.
        {
            // 위치와 회전값을 설정합니다.
            Enemy->SetActorLocationAndRotation(Location, Rotation);
            
            // !! 핵심 !!: 프로필 정보를 넘겨 모든 상태(스탯, 색깔, 강화)를 한번에 초기화합니다.
            Enemy->InitializeEnemy(Profile);
            
            // 액터를 다시 보이게 하고, 충돌과 틱을 활성화합니다.
            Enemy->SetActorHiddenInGame(false);
            Enemy->SetActorEnableCollision(true);
            Enemy->SetActorTickEnabled(true);

            UE_LOG(LogTemp, Log, TEXT("Reused an enemy (%s) from the pool!"), *Profile.EnemyClass->GetName());
            return Enemy; // 재사용했으니 즉시 반환
        }
    }

    // 3. 재사용할 적이 없다면 새로 스폰합니다.
    UE_LOG(LogTemp, Warning, TEXT("No available enemy in pool for %s. Spawning a new one."), *Profile.EnemyClass->GetName());

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AEnemyBase* NewEnemy = GetWorld()->SpawnActor<AEnemyBase>(Profile.EnemyClass, Location, Rotation, Params);
    if (NewEnemy)
    {
        // 새로 생성한 적을 풀에 추가합니다.
        Pool.PooledEnemies.Add(NewEnemy);
        
        // !! 핵심 !!: 새로 만든 적도 동일한 함수로 상태를 초기화합니다.
        NewEnemy->InitializeEnemy(Profile);
    }
    return NewEnemy;
}
     

// 적을 풀로 반납 (서버에서만 실행)
void AEnemyPoolManager::ReturnEnemy(AEnemyBase* Enemy) const
{
    if (!HasAuthority() || !Enemy) return;

    Enemy->Deactivate();
}

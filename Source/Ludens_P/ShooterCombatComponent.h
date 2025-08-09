#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterCombatComponent.generated.h"

class AShooterEnemyBase; // 전방 선언

UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class LUDENS_P_API UShooterCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UShooterCombatComponent();

    // BT Task 등에서 호출
    UFUNCTION(BlueprintCallable, Category="Combat")
    bool CanFire() const;

    UFUNCTION(BlueprintCallable, Category="Combat")
    bool TryFire(AActor* Target); // 서버에서만 true

    // 연발(슈터형+/머신건) 파라미터
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat|Fire")
    int32 BurstCount = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat|Fire")
    float BurstInterval = 0.06f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat|Fire")
    float FireCooldown = 0.4f;

protected:
    virtual void BeginPlay() override;

private:
    // Owner 캐시
    UPROPERTY() AShooterEnemyBase* OwnerEnemy = nullptr;

    // 상태
    bool bFiring = false;
    int32 ShotsLeftInBurst = 0;
    TWeakObjectPtr<AActor> CachedTarget;

    FTimerHandle BurstHandle;
    FTimerHandle CooldownHandle;

    void DoSingleShot();
    void EndBurst();
    void ResetCooldown();
};

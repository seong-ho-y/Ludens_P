#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterCombatComponent.generated.h"

UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class LUDENS_P_API UShooterCombatComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;
    //BT에서 호출하는 로직 딱 하나
    UFUNCTION(BlueprintCallable) bool TryFire(AActor* Target);

    //파라미터
    UPROPERTY(EditAnywhere) float FireCooldown = 0.3f;
    UPROPERTY(EditAnywhere) int32 BurstCount = 1;
    UPROPERTY(EditAnywhere) float BurstInterval = 0.06f;

    //디버그 토글
    UPROPERTY(EditAnywhere) bool bDebugLog = false;

protected:

private:
    enum class EFireState : uint8 {Idle, Burst, Cooldown};
    EFireState State = EFireState::Idle;

    TWeakObjectPtr<AActor> CachedTarget;
    int32 ShotsLeft = 0;
    FTimerHandle BurstTimer;
    FTimerHandle CooldownTimer;

    //의존성 : 적 전용 무기 컴포넌트(실제 스폰 담당)
    //전방선언 후 c++에서 include
    UPROPERTY() class UEnemyProjectileWeaponComponent* Weapon = nullptr;

    bool CanFire() const;
    bool DoSingleShot();          // 성공 시 true
    void BeginBurst(AActor* Target);
    void TickBurst();             // 타이머 콜백
    void BeginCooldown();
    void EndCooldown();
    void Log(const TCHAR* Fmt, ...) const;  // bDebugLog일 때만 출력
};


// MeleeAttackComponent.cpp

#include "EnemyMeleeComp.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Actor.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/Controller.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

UEnemyMeleeComp::UEnemyMeleeComp()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UEnemyMeleeComp::BeginPlay()
{
    Super::BeginPlay();
    Owner = GetOwner();
}

void UEnemyMeleeComp::Attack()
{
    if (!Owner) return;

    // ✨ 서버에서만 데미지 로직 실행
    if (Owner->HasAuthority())
    {
        const FVector AttackLocation = Owner->GetActorLocation() + (Owner->GetActorForwardVector() * 100.0f);
        const FRotator AttackRotation = Owner->GetActorRotation();

        // 모든 클라이언트에 VFX 재생 명령
        Multicast_SpawnAttackVFX(AttackLocation, AttackRotation);

        // ✅ 보여주신 코드처럼 ApplyRadialDamage를 사용하여 한 번에 처리합니다.
        UGameplayStatics::ApplyRadialDamage(
            GetWorld(),                      // 월드
            AttackDamage,                    // 데미지 양
            AttackLocation,                  // 데미지 발생 위치
            AttackRadius,                    // 데미지 반경
            UDamageType::StaticClass(),      // 데미지 타입
            TArray<AActor*>(),               // 무시할 액터 배열 (여기서는 Owner를 아래에 지정)
            Owner,                           // 데미지를 유발한 액터
            Owner->GetInstigatorController(),// 가해자 컨트롤러
            true,                            // 데미지 감쇠 여부 (거리에 따라 데미지 감소)
            ECollisionChannel::ECC_Visibility// 데미지가 벽을 뚫는지 결정하는 채널
        );
    }
}

// Multicast 함수 (수정 없음)
void UEnemyMeleeComp::Multicast_SpawnAttackVFX_Implementation(const FVector& Location, const FRotator& Rotation)
{
    if (AttackVFX)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), AttackVFX, Location, Rotation);
    }
}

// 리플리케이션 설정 (수정 없음)
void UEnemyMeleeComp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
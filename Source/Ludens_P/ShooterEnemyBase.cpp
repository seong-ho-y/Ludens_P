// 슈터Enemy의 기본 클래스
// 기본적으로 ShooterAIComponent, ShootingComponent, WalkerAIComponent를 갖고 있음
// FireAt메서드를 가지고 있으며 Actor형 포인터 Target을 인자로 받아서 ShootingComponent의 FireAt호출

#include "ShooterEnemyBase.h"

#include "ShooterCombatComponent.h"

AShooterEnemyBase::AShooterEnemyBase()
{
	ShooterCombatComponent = CreateDefaultSubobject<UShooterCombatComponent>(TEXT("ShooterCombatComponent"));
	ShootingComponent = CreateDefaultSubobject<UShootingComponent>(TEXT("ShootingComponent"));
}


void AShooterEnemyBase::FireAt(AActor* Target)
{
	UE_LOG(LogTemp, Warning, TEXT("FireAt called on %s (Authority: %d)"), *GetName(), HasAuthority());

	if (HasAuthority())
	{
		Server_FireAt(Target); // 서버면 직접 실행
	}
	else
	{
		// 안전하게: 클라에서 호출됐다면 서버로 요청
		Server_FireAt(Target);
	}
}

void AShooterEnemyBase::Server_FireAt_Implementation(AActor* Target)
{
	if (!IsValid(Target)) return;

	// 여기서 실제 프로젝타일 스폰/라인트레이스/데미지 적용
	// SpawnActor<...>(...), SetVelocity(...), ApplyDamage(...) 등
	// ...

	Multicast_OnFiredFX();
}

void AShooterEnemyBase::Multicast_OnFiredFX_Implementation()
{
	// 총구 섬광, 사운드, 카메라쉐이크 등 클라 연출
}

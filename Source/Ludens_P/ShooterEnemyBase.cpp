// 슈터Enemy의 기본 클래스
// 기본적으로 ShooterAIComponent, ShootingComponent, WalkerAIComponent를 갖고 있음
// FireAt메서드를 가지고 있으며 Actor형 포인터 Target을 인자로 받아서 ShootingComponent의 FireAt호출

#include "ShooterEnemyBase.h"

#include "ShooterAIComponent.h"

AShooterEnemyBase::AShooterEnemyBase()
{
	ShooterAIComponent = CreateDefaultSubobject<UShooterAIComponent>(TEXT("ShooterAIComponent"));
	ShootingComponent = CreateDefaultSubobject<UShootingComponent>(TEXT("ShootingComponent"));
}

void AShooterEnemyBase::FireAt(AActor* Target)
{
	UE_LOG(LogTemp, Log, TEXT("FireAt at ShooterEnemy"));
	if (ShootingComponent)
	{
		ShootingComponent->FireAt(Target);
	}
}
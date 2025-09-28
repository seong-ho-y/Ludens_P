// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectile.h"

#include "EnemyPMComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Ludens_P/EnemyBase.h"

// Sets default values
AEnemyProjectile::AEnemyProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &::AEnemyProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UEnemyPMComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 1.f;
	ProjectileMovement->MaxSpeed = 1.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	ProjectileMovement->ProjectileGravityScale = 0.f;
	// Die after 3 seconds by default
	InitialLifeSpan = 5.0f;
}

void AEnemyProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 나 자신이나 나를 쏜 소유자(Owner)와 부딪힌 경우는 무시합니다.
	AActor* MyOwner = GetOwner();
	if (OtherActor == nullptr || OtherActor == this || OtherActor == MyOwner)
	{
		return;
	}

	// 충돌 설정이 올바르다면, 이 함수는 플레이어 또는 월드(벽)와 부딪혔을 때만 호출됩니다.
	// 따라서 적 클래스인지 확인할 필요가 없습니다.

	// 1. 부딪힌 대상이 플레이어인지 확인합니다. 
	// (만약 플레이어 클래스가 ACharacter가 아닌 직접 만든 클래스라면 해당 클래스로 캐스팅해야 합니다. 예: AMyPlayerCharacter)
	ACharacter* PlayerCharacter = Cast<ACharacter>(OtherActor);
	if (PlayerCharacter && !Cast<AEnemyBase>(OtherActor)) // 플레이어 캐릭터는 맞지만, Enemy는 아닐 경우
	{
		// 2. 데미지 처리를 위한 정보를 준비합니다.
		const float DamageAmount = 10.0f; // 이 프로젝타일의 데미지
		AController* InstigatorController = MyOwner ? MyOwner->GetInstigatorController() : nullptr;

		// 3. 언리얼 데미지 시스템을 통해 플레이어에게 데미지를 전달합니다.
		UGameplayStatics::ApplyDamage(
			PlayerCharacter,
			DamageAmount,
			InstigatorController,
			this,
			nullptr
		);
	}
	
	// 4. 물리 시뮬레이션 중인 다른 오브젝트에 대한 로직은 유지할 수 있습니다.
	if (OtherComp != nullptr && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}

	// 5. 플레이어에게 닿았거나, 벽과 같은 다른 무언가에 부딪혔으므로 이제 프로젝타일을 파괴합니다.
	Destroy();
}

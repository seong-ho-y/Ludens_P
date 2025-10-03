// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ludens_PProjectile.h"

#include "EnemyBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState_Real.h"
#include "GameFramework/Character.h"

ALudens_PProjectile::ALudens_PProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &ALudens_PProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
	
}

void ALudens_PProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 나 자신이나 나를 쏜 소유자(Owner)와 부딪힌 경우는 무시합니다.
	AActor* MyOwner = GetOwner();
	if (OtherActor == nullptr || OtherActor == this || OtherActor == MyOwner)
	{
		return;
	}
	
	// 안전하게 캐스팅하여 할당
	PSR = Cast<APlayerState_Real>(Cast<ACharacter>(GetOwner())->GetPlayerState());

	if (!PSR)
	{
		UE_LOG(LogTemp, Error, TEXT("[PlayerStateComponent] PSR is nullptr!"));
		return;
	}
	
	// 1. 부딪힌 대상이 적인지(AEnemyBase를 상속받았는지) 확인합니다.
	AEnemyBase* HitEnemy = Cast<AEnemyBase>(OtherActor);
	if (HitEnemy)
	{
		// 2. 데미지 처리를 위한 정보를 준비합니다.
		const float DamageAmount = PSR->AttackDamage; // 이 프로젝타일의 기본 데미지
		AController* InstigatorController = MyOwner ? MyOwner->GetInstigatorController() : nullptr; // 나를 쏜 폰의 컨트롤러

		// 3. 언리얼 데미지 시스템을 통해 데미지를 전달합니다.
		// 이 함수가 호출되면 서버에 있는 HitEnemy의 TakeDamage 함수가 실행됩니다.
		UGameplayStatics::ApplyDamage(
			HitEnemy,
			DamageAmount,
			InstigatorController, // "누가" 쐈는지에 대한 정보 (색상 판별에 사용)
			this,                 // "무엇으로" 때렸는지에 대한 정보 (프로젝타일 자신)
			nullptr               // 데미지 타입 클래스 (옵션)
		);
	}
    
	// 4. 물리 시뮬레이션 중인 다른 오브젝트에 대한 기존 로직도 유지할 수 있습니다.
	if (OtherComp != nullptr && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}

	// 5. 적에게 닿았거나, 다른 무언가에 부딪혔으므로 이제 프로젝타일을 파괴합니다.
	Destroy();
}
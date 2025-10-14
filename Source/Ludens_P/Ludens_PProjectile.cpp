// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ludens_PProjectile.h"

#include "EnemyBase.h"
#include "PlayerState_Real.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

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
	InitialLifeSpan = 10.0f;
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
		UE_LOG(LogTemp, Error, TEXT("[PlayerStateComponent] PSR is nullptr! in Projectile"));
		return;
	}
	
	// 1. 부딪힌 대상이 적인지(AEnemyBase를 상속받았는지) 확인합니다.
	AEnemyBase* HitEnemy = Cast<AEnemyBase>(OtherActor);
	if (HitEnemy)
	{
		// 2. 데미지 처리를 위한 정보를 준비합니다.
		float DamageAmount = PSR->AttackDamage;
		bool bIsCritical = false; // bool 타입 변수를 확인해서 크리티컬이면 이를 시각화 하는데 쓸 수 있음. 물론 안 써도 상관 X
       
		// FMath::FRand()는 0.0f와 1.0f 사이의 랜덤 float 값을 반환합니다.
		if (FMath::FRand() < PSR->CriticalRate) // 예: CriticalRate가 0.1이면 10% 확률로 true
		{
			// 크리티컬 발동!
			bIsCritical = true;
          
			// 기본 데미지에 크리티컬 데미지 배율을 곱합니다.
			// PSR->CriticalDamage는 배율 값(초깃값: 1.5배)
			DamageAmount *= PSR->CriticalDamage;
          
			// (선택 사항: 크리티컬 발생 로그를 남기거나, 클라이언트에게 알리는 로직 추가)
			UE_LOG(LogTemp, Warning, TEXT("Critical Hit! Damage: %f"), DamageAmount);
		}
		
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
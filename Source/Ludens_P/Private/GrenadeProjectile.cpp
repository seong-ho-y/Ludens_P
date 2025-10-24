// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeProjectile.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGrenadeProjectile::AGrenadeProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// ⭐ 멀티플레이어에서 가장 중요한 설정!
	// 이 액터가 서버에서 스폰되면 클라이언트에도 복제되도록 합니다.
	bReplicates = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = CollisionComp;
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
	// 충돌 이벤트 함수(OnHit)를 바인딩합니다.
	CollisionComp->OnComponentHit.AddDynamic(this, &AGrenadeProjectile::OnHit);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
}

// Called when the game starts or when spawned
void AGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGrenadeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGrenadeProjectile::SpawnExploVFX_Implementation()
{
	if (ExplosionVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionVFX, GetActorLocation());
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetOwner()->GetActorLocation());
	}
}

void AGrenadeProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// ⭐ 모든 로직은 권한을 가진 서버에서만 실행합니다.
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("Grenade OnHit Called : Server"));
		// 나 자신이나 나를 쏜 소유자(Owner)와 부딪힌 경우는 무시합니다.
		AActor* MyOwner = GetOwner();
		if (OtherActor == nullptr || OtherActor == this || OtherActor == MyOwner)
		{
			return;
		}
		SpawnExploVFX();
		// 폭발 이펙트와 사운드를 모든 클라이언트에서 재생하도록 합니다. (Multicast RPC 사용 - 선택사항)
		// Multicast_PlayExplosionEffects();

		AController* InstigatorController = GetInstigatorController();

		// 범위 내의 모든 Damageable 액터에게 피해를 줍니다.
		UGameplayStatics::ApplyRadialDamage(
			this,                 // World Context Object
			DamageAmount,         // 기본 피해량
			GetActorLocation(),   // 폭발 원점
			DamageRadius,         // 폭발 반경
			nullptr,              // DamageType Class (기본값 사용)
			TArray<AActor*>(),    // 무시할 액터 목록
			this,                 // 피해를 유발한 액터 (수류탄 자신)
			InstigatorController, // 피해를 입힌 컨트롤러 (플레이어)
			true                  // 풀 데미지 여부
		);

		// 피해를 주고 난 후 프로젝타일을 파괴합니다.
		// Destroy()는 자동으로 복제되어 모든 클라이언트에서도 파괴됩니다.
		Destroy();
	}
}

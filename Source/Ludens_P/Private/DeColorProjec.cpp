// Fill out your copyright notice in the Description page of Project Settings.


#include "DeColorProjec.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADeColorProjec::ADeColorProjec()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = CollisionComp;
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
	// 충돌 이벤트 함수(OnHit)를 바인딩합니다.
	CollisionComp->OnComponentHit.AddDynamic(this, &ADeColorProjec::OnHit);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 5000.f;
	ProjectileMovement->MaxSpeed = 5000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void ADeColorProjec::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADeColorProjec::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADeColorProjec::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority()) return;
	UE_LOG(LogTemp, Log, TEXT("DeColor OnHit Called : Server"));
	// 나와 발사자를 제외한 대상에게만 피해를 줍니다.
	if (OtherActor && OtherActor != this && OtherActor != GetOwner())
	{
		// 1. VFX
		SpawnExploVFX();

		// 2. 피해를 입었다는 사실을 알립니다.
		UGameplayStatics::ApplyDamage(
			OtherActor,          // 피해를 입을 액터
			DamageAmount,                // 기본 피해량 (0으로 설정)
			GetInstigatorController(), // 피해를 유발한 컨트롤러 (플레이어)
			this,                // 피해를 유발한 액터 (프로젝타일 자신)
			nullptr              // 데미지 타입 클래스
		);
	}
	Destroy();
}

void ADeColorProjec::SpawnExploVFX_Implementation()
{
	if (ExploVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExploVFX, GetActorLocation());
	}
}


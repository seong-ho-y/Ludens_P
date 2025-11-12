// Fill out your copyright notice in the Description page of Project Settings.


#include "HealPack.h"

#include "PlayerStateComponent.h"
#include "PlayerState_Real.h"
#include "ReviveComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Ludens_P/Ludens_PCharacter.h"

// Sets default values
AHealPack::AHealPack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	bReplicates = true;
	
	PrimaryActorTick.bCanEverTick = true;
	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	RootComponent = Collision;
	Collision->SetCollisionProfileName("Projectile");

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->bShouldBounce = true;       // 튕김 활성화
	Movement->Bounciness = 0.6f;          // 튕김 세기 (0~1)
	Movement->Friction = 0.3f;            // 마찰력
	Movement->ProjectileGravityScale = 1.0f;
	Movement->InitialSpeed = 1200.f;
	Movement->MaxSpeed = 1200.f;
	Movement->bRotationFollowsVelocity = true;  // 회전 방향 따라가게
	Movement->BounceVelocityStopSimulatingThreshold = 10.f;  // 10cm/s 이하에서 멈춤
	
}

// Called when the game starts or when spawned
void AHealPack::BeginPlay()
{
	Super::BeginPlay();
	Collision->OnComponentHit.AddDynamic(this, &AHealPack::OnHit);
}

// Called every frame
void AHealPack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AHealPack::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
								   UPrimitiveComponent* OtherComp, FVector NormalImpulse,
								   const FHitResult& Hit)
{
	if (!HasAuthority()) return;
	// 1️⃣ 플레이어에 닿으면 회복
	if (ALudens_PCharacter* Player = Cast<ALudens_PCharacter>(OtherActor))
	{
		UPlayerStateComponent* PSC = Player->GetComponentByClass<UPlayerStateComponent>();
		APlayerState_Real* PSR = Player->PSR;
		UReviveComponent* RevC = Player->GetComponentByClass<UReviveComponent>();
		if (PSC->IsKnocked) RevC->HandleReviveComplete();
		PSC->CurrentHP += 50;
		SpawnHealVFX();
		Destroy();
	}

}

void AHealPack::SpawnHealVFX_Implementation()
{
	if (HealVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), HealVFX, GetActorLocation());
	}
	if (HealSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), HealSound, GetActorLocation());
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GrenadeComp.h"

#include "GrenadeProjectile.h"

// Sets default values for this component's properties
UGrenadeComp::UGrenadeComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	CooldownTime = 15.0f;
	bIsOnCooldown = false;
	// ...
}


// Called when the game starts
void UGrenadeComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

bool UGrenadeComp::CanUseTool_Implementation() const
{
	return !bIsOnCooldown;
}

void UGrenadeComp::PerformToolAction(APawn* InstigatorPawn)
{
	Super::PerformToolAction(InstigatorPawn);
	if (GetOwnerRole() == ROLE_Authority)
	{
		Server_StartCooldown();
	}
	else
	{
		Server_StartCooldown();
	}
	UE_LOG(LogTemp, Log, TEXT("Grenade Tool Fired!"));
	PlayToolSound(InstigatorPawn); // 공통 재생 함수 호출
	Server_ThrowGrenade();
}


// 서버에서 실행되는 실제 발사 로직
void UGrenadeComp::Server_ThrowGrenade_Implementation()
{
	if (!GrenadeProjectileClass) return;

	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	// 발사 위치와 방향을 계산합니다 (카메라 기준).
	FVector CameraLocation;
	FRotator CameraRotation;
	MyOwner->GetActorEyesViewPoint(CameraLocation, CameraRotation);

	// 카메라 위치보다 살짝 앞에서 스폰하여 플레이어와 충돌하는 것을 방지합니다.
	FVector MuzzleLocation = CameraLocation + CameraRotation.Vector() * 100.0f;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = MyOwner;
	SpawnParams.Instigator = MyOwner->GetInstigator(); // 피해를 입힌 Pawn
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector SpawnLocation = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.f;
	FRotator SpawnRotation = CameraRotation;
	
	SpawnRotation.Pitch += 30.f;

	FTransform SpawnTransform(SpawnRotation, SpawnLocation);
	// 서버 월드에 프로젝타일을 스폰합니다.
	GetWorld()->SpawnActor<AGrenadeProjectile>(GrenadeProjectileClass, SpawnTransform, SpawnParams);
}
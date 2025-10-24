// Fill out your copyright notice in the Description page of Project Settings.


#include "HealPackComp.h"

#include "HealPack.h"

// Sets default values for this component's properties
UHealPackComp::UHealPackComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	CooldownTime = 30.0f;
	bIsOnCooldown = false;

	// ...
}


// Called when the game starts
void UHealPackComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


void UHealPackComp::PerformToolAction(APawn* InstigatorPawn)
{
	Super::PerformToolAction(InstigatorPawn);
	Server_ThrowHeal();
}

// Called every frame
void UHealPackComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealPackComp::Server_ThrowHeal_Implementation()
{
	if (HealPack)
	{
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
		GetWorld()->SpawnActor<AHealPack>(HealPack, SpawnTransform, SpawnParams);
	}
}


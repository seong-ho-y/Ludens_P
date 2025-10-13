// Fill out your copyright notice in the Description page of Project Settings.


#include "DecolorComp.h"

#include "DeColorProjec.h"

// Sets default values for this component's properties
UDecolorComp::UDecolorComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDecolorComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDecolorComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDecolorComp::Interact_Implementation(APawn* InstigatorPawn)
{
	IToolInterface::Interact_Implementation(InstigatorPawn);
	Server_Shoot();
}

void UDecolorComp::Server_Shoot_Implementation()
{
	if (!DeColorProjectile) return;
	AActor* MyOwner = GetOwner();
	if (!MyOwner) return;

	// 발사 위치와 방향을 계산합니다 (카메라 기준).
	FVector CameraLocation;
	FRotator CameraRotation;
	MyOwner->GetActorEyesViewPoint(CameraLocation, CameraRotation);

	// 카메라 위치보다 살짝 앞에서 스폰하여 플레이어와 충돌하는 것을 방지합니다.
	FVector MuzzleLocation = CameraLocation + CameraRotation.Vector() * 100.0f;

	FTransform SpawnTransform(CameraRotation, MuzzleLocation);
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = MyOwner;
	SpawnParams.Instigator = MyOwner->GetInstigator(); // 피해를 입힌 Pawn
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 서버 월드에 프로젝타일을 스폰합니다.
	GetWorld()->SpawnActor<ADeColorProjec>(DeColorProjectile, SpawnTransform, SpawnParams);
}


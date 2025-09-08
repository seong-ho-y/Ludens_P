// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectileWeaponComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values for this component's properties

void UEnemyProjectileWeaponComponent::Server_FireAt_Implementation(AActor* Target)
{
	if (!Target || !ProjectileClass) return; //nullptr 예외처리
	AActor* Owner = GetOwner(); if (!Owner) return; //nullptr 예외처리

	//프로젝타일 발사될 곳(Loc, Rot)정하기
	FVector Loc; FRotator Rot;
	if (auto* Mesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
		Mesh && Mesh -> DoesSocketExist(MuzzleSocket)) //요까지 조건문
	{
		Loc = Mesh->GetSocketLocation(MuzzleSocket); //총구 있을 경우 거기로
	} else
	{
		Loc = Owner->GetActorLocation(); //총구 따로 없으면 Owner의 Loc으로
	}
	Rot = (Target->GetActorLocation() - Loc).Rotation(); //Rot은 Target과의 거리 계산해서

	//스폰 파라미터 설정 (스폰 파라미터란 스폰 될 때 세부적인 디테일을 담은 것)
	FActorSpawnParameters P;
	P.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	P.Owner = Owner; P.Instigator = Cast<APawn>(Owner);

	if (AActor* Proj = GetWorld()->SpawnActor<AActor>(ProjectileClass, Loc, Rot, P))
	{
		if (auto* PMC = Proj->FindComponentByClass<UProjectileMovementComponent>())
		{
			PMC->Velocity = Rot.Vector() * MuzzleSpeed; // 속도 설정
			PMC->InitialSpeed = PMC->MaxSpeed = MuzzleSpeed;
		}
		Proj->SetReplicates(true); //멀티 복제 설정
		
		//얘는 나중에 FX 함수 구현됐을 때
		Multicast_FireFX(Loc, Rot);
	}
}

void UEnemyProjectileWeaponComponent::Multicast_FireFX_Implementation(FVector Loc, FRotator Rot)
{
	// 총구 섬광/사운드 등 필요시
}
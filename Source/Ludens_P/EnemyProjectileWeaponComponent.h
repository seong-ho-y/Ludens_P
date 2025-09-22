// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyProjectileWeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UEnemyProjectileWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere) TSubclassOf<AActor> ProjectileClass;
	UPROPERTY(EditAnywhere) FName MuzzleSocket = "Muzzle";
	UPROPERTY(EditAnywhere) float MuzzleSpeed = 3000.f;

	//BT에서 호출할 메서드
	UFUNCTION(Server, Reliable) void Server_FireAt(AActor* Target);
	
	UFUNCTION(NetMulticast, UnReliable) void Multicast_FireFX(FVector Loc, FRotator Rot);
		
};

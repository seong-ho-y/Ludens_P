// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraSystem.h"
#include "HealPack.generated.h"

class UBoxComponent;
class UNiagaraSystem;
class UProjectileMovementComponent;

UCLASS()
class LUDENS_P_API AHealPack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHealPack();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* Collision;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* Movement;

	UPROPERTY(EditDefaultsOnly, Category = "Heal")
	float HealAmount = 50.f;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* HealVFX;
	

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
			   UPrimitiveComponent* OtherComp, FVector NormalImpulse,
			   const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnHealVFX();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};

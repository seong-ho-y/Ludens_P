// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrenadeProjectile.generated.h"

class UNiagaraSystem;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class LUDENS_P_API AGrenadeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenadeProjectile();

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;
	
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
	
	// 폭발 시 피해량과 범위를 설정할 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float DamageAmount = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
	float DamageRadius = 300.0f;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* ExplosionVFX;

	
	UPROPERTY(EditDefaultsOnly, Category = "Tool|Sound")
	USoundBase* ExplosionSound; // 수류탄 폭발 사운드

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse,
	           const FHitResult& Hit);

	UFUNCTION(NetMulticast, Reliable)
	void SpawnExploVFX();
	

};

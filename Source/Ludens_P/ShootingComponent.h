// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "ShootingComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LUDENS_P_API UShootingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UShootingComponent();
	
	void FireAt(AActor* Target);

	UPROPERTY(EditAnywhere, Category="Shooting")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category="Shooting")
	float MuzzleOffset = 70.f;

	UPROPERTY(EditAnywhere, Category="Shooting")
	float FireCooldown = 0.5f;

private:
	bool bCanFire = true;
	FTimerHandle FireCooldownHandle;

	void ResetFireCooldown();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterAIComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LUDENS_P_API UShooterAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category="AI")
	float FireRange = 800.f;

	UPROPERTY(EditAnywhere, Category="AI")
	float FireCooldown = 2.0f;

	bool bCanFire = true;
	FTimerHandle FireCooldownHandle;

	class AEnemyBase* OwnerEnemy;
	class UWalkerAIComponent* WalkerComponent;

	void TryFire();
	void ResetCanFire();
};

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
	UShooterAIComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, Category="AI")
	float FireRange = 1500.f; //사격사거리 설정

	UPROPERTY(EditAnywhere, Category="AI")
	float FireCooldown = 0.5f; //공격 쿨타임

	bool bCanFire = true;
	FTimerHandle FireCooldownHandle;

	class AShooterEnemyBase* OwnerEnemy;
	UWalkerAIComponent* WalkerComponent; //컴포넌트는 class로 선언할 필요가 없는 듯

	void TryFire(AActor* Target);
	void ResetCanFire();
};

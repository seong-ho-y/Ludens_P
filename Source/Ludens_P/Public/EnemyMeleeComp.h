// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Components/ActorComponent.h"
#include "EnemyMeleeComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UEnemyMeleeComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyMeleeComp();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnAttackVFX(const FVector& Vector, const FRotator& Rotator);
	void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void Attack();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 이 컴포넌트를 소유한 액터
	UPROPERTY()
	AActor* Owner;

	// 공격 데미지 (에디터에서 수정 가능)
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackDamage = 25.0f;

	// 공격 범위 반경 (에디터에서 수정 가능)
	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackRadius = 150.0f;

	// 공격 시 재생할 VFX 파티클 (에디터에서 수정 가능)
	UPROPERTY(EditAnywhere, Category = "Attack")
	UNiagaraSystem* AttackVFX;
};

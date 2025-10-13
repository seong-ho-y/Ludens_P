// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToolInterface.h"
#include "Components/ActorComponent.h"
#include "GrenadeComp.generated.h"


class UNiagaraSystem;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UGrenadeComp : public UActorComponent, public IToolInterface
{
	GENERATED_BODY()
	

	// Sets default values for this component's properties
	UGrenadeComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	// 서버에서만 실행될 수류탄 발사 함수
	UFUNCTION(Server, Reliable)
	void Server_ThrowGrenade();

	// 수류탄으로 스폰할 프로젝타일 블루프린트 또는 C++ 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AGrenadeProjectile> GrenadeProjectileClass;
};

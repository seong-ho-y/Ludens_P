// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToolBaseComponent.h"
#include "ToolInterface.h"
#include "Components/ActorComponent.h"
#include "GrenadeComp.generated.h"


class AGrenadeProjectile;
class UNiagaraSystem;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UGrenadeComp : public UToolBaseComponent
{
	GENERATED_BODY()
	

	// Sets default values for this component's properties
	UGrenadeComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	bool CanUseTool_Implementation() const;

public:	
	virtual void PerformToolAction(APawn* InstigatorPawn) override;

	UFUNCTION(Server, Reliable)
	void Server_ThrowGrenade();

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AGrenadeProjectile> GrenadeProjectileClass;

};

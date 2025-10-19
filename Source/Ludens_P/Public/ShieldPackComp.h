// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToolInterface.h"
#include "Components/ActorComponent.h"
#include "ShieldPackComp.generated.h"


class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UShieldPackComp : public UActorComponent, public IToolInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShieldPackComp();

protected:

	
	// Called when the game starts
	virtual void BeginPlay() override;

	FTimerHandle ShieldDurationHandle;

	UPROPERTY(EditAnywhere, Category = "Shield")
	float ShieldAmount = 50.f;

	UPROPERTY(EditAnywhere, Category = "Shield")
	float ShieldDuration = 5.f;

	float OriginalMaxShield = 100.f;

	bool bIsShieldBuffActive = false;

	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* ShiledVFX;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Interact_Implementation(APawn* InstigatorPawn) override;

	
	void RestoreShield();

	UFUNCTION(Server, Reliable)
	void Server_GetShield();

	UFUNCTION(NetMulticast, Reliable)
	void SpawnShieldVFX();

	

};

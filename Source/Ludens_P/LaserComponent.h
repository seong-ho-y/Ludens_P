// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "LaserComponent.generated.h"

class UNiagaraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API ULaserComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULaserComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Laser")
	UNiagaraComponent* LaserComp;
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetLaserActive(bool bLaserActive);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateLaserTarget(const FVector& TargetLocation);
	
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void TurnOn(const FVector& TargetLocation);
	void TurnOff();

	void UpdateLaserTarget(const FVector& TargetLocation);
		
};

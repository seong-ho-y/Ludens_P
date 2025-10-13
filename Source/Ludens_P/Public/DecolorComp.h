// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToolInterface.h"
#include "Components/ActorComponent.h"
#include "DecolorComp.generated.h"


class ADeColorProjec;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UDecolorComp : public UActorComponent, public IToolInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDecolorComp();

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<ADeColorProjec> DeColorProjectile;  

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Interact_Implementation(APawn* InstigatorPawn) override;

	UFUNCTION(Server, Reliable)
	void Server_Shoot();
};

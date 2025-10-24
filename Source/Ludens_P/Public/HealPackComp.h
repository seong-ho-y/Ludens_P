// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToolBaseComponent.h"
#include "ToolInterface.h"
#include "Components/ActorComponent.h"
#include "HealPackComp.generated.h"


class AHealPack;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UHealPackComp : public UToolBaseComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealPackComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UFUNCTION(Server, Reliable)
	void Server_ThrowHeal();

	virtual void PerformToolAction(APawn* InstigatorPawn) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Proj")
	TSubclassOf<AHealPack> HealPack;

};

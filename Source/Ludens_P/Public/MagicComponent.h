// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Components/ActorComponent.h"
#include "MagicComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UMagicComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMagicComponent();

	UFUNCTION(BlueprintCallable, Category="Magic")
	void CastSpellAtLocation(const FVector& TargetLocation);
	//void PrepareToCast(const FVector& Vector);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic")
	float CastingTime = 0.7f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic")
	float SpellDamage = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic")
	float SpellRadius = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic")
	UMaterialInterface* WarningDecalMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic")
	UNiagaraSystem* SpellEffect;
	FVector TargetCastLocation = FVector::ZeroVector;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SpawnWarningDecal(const FVector TargetLocation);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySpellEffect(const FVector& Location);

private:
	void ExecuteSpell(FVector Location);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitFeedbackComponent.generated.h"

class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UHitFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void BeginPlay() override;
	// Sets default values for this component's properties
	UHitFeedbackComponent();

	void PlayHitEffects(const FVector& HitLocation);


public:
	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UNiagaraSystem* HitVFX;
	
protected:
	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitEffects(const FVector& HitLocation);

	void StartHitFlash();
	void EndHitFlash();
	FTimerHandle HitFlashTimer;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	UMaterialInstanceDynamic* BodyMID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Feedback|Flash")
	FName EmissiveParamName = TEXT("EmissivePower");
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Feedback|Flash")
	float FlashIntensity = 20.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Feedback|Flash")
	float FlashDuration = 0.1f;

public:	

		
};

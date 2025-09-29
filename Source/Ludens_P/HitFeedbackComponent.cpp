// Fill out your copyright notice in the Description page of Project Settings.


#include "HitFeedbackComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties

void UHitFeedbackComponent::BeginPlay()
{
	Super::BeginPlay();
}

UHitFeedbackComponent::UHitFeedbackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	// ...
}

void UHitFeedbackComponent::PlayHitEffects(const FVector& HitLocation)
{
	AActor* MyOwner = GetOwner();
	if (!MyOwner || !MyOwner->HasAuthority())
	{
		return;
	}
	Multicast_PlayHitEffects(HitLocation);
}

void UHitFeedbackComponent::Multicast_PlayHitEffects_Implementation(const FVector& HitLocation)
{
	if (!HitVFX)
	{
		//UE_LOG(LogTemp, Error, TEXT("HitVFX is Not Set!"));
		return;
	}

	if (UWorld* World = GetWorld())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(World, HitVFX, HitLocation);
	}
    
	StartHitFlash();
}
void UHitFeedbackComponent::StartHitFlash()
{
	// BodyMID가 없으면 Owner로부터 찾아옵니다.
	if (!BodyMID)
	{
		//UE_LOG(LogTemp, Display, TEXT("No BodyMID"));
		return;
		/*
		ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
		if (OwnerChar && OwnerChar->GetMesh())
		{
			BodyMID = OwnerChar->GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
		}
		*/
	}

	if (BodyMID)
	{
		//UE_LOG(LogTemp, Display, TEXT("Starting HitFlash"));
		BodyMID->SetScalarParameterValue(EmissiveParamName, FlashIntensity);
		GetWorld()->GetTimerManager().SetTimer(HitFlashTimer, this, &UHitFeedbackComponent::EndHitFlash, FlashDuration, false);
	}
}

void UHitFeedbackComponent::EndHitFlash()
{
	if (BodyMID)
	{
		BodyMID->SetScalarParameterValue(EmissiveParamName, 0.0f);
	}
}
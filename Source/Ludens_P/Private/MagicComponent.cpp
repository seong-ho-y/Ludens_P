// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UMagicComponent::UMagicComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMagicComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UMagicComponent::CastSpellAtLocation(const FVector TargetLocation)
{
	Multicast_SpawnWarningDecal(TargetLocation);

	FTimerHandle SpellTimerHandle;
	FTimerDelegate SpellDelegate = FTimerDelegate::CreateUObject(this, &UMagicComponent::ExecuteSpell, TargetLocation);
	GetWorld()->GetTimerManager().SetTimer(SpellTimerHandle, SpellDelegate, CastingTime, false);
}


void UMagicComponent::Multicast_SpawnWarningDecal_Implementation(const FVector TargetLocation)
{
	if (WarningDecalMaterial)
	{
		FRotator DecalRotation = FRotator(90.f, FMath::RandRange(-50.f, 50.f),0.f);
		UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), WarningDecalMaterial, FVector(SpellRadius), TargetLocation, DecalRotation, CastingTime);
	}
}



void UMagicComponent::Multicast_PlaySpellEffect_Implementation(const FVector& Location)
{
	if (SpellEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpellEffect, Location);
	}
}

void UMagicComponent::ExecuteSpell(FVector Location)
{
	Multicast_PlaySpellEffect(Location);
	UGameplayStatics::ApplyRadialDamage(
		GetWorld(),
		SpellDamage,
		Location,
		SpellRadius,
		UDamageType::StaticClass(),
		TArray<AActor*>(),
		GetOwner(),
		GetOwner()->GetInstigatorController()
		);
}


// Called every frame
void UMagicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


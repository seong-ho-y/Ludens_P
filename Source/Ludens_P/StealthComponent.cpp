// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthComponent.h"

// Sets default values for this component's properties
UStealthComponent::UStealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// StealthComponent.cpp

void UStealthComponent::BeginPlay()
{
    Super::BeginPlay();
    PrimaryComponentTick.bCanEverTick = true;
    ActivateStealth();
}

void UStealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    TimeSinceLastCheck += DeltaTime;
    if (TimeSinceLastCheck >= StealthCheckInterval)
    {
        TimeSinceLastCheck = 0.0f;

        AActor* NearestPlayer = FindNearestPlayer();
        if (NearestPlayer)
        {
            float Dist = FVector::Dist(NearestPlayer->GetActorLocation(), GetOwner()->GetActorLocation());

            if (Dist < RevealDistance && bIsStealthed)
            {
                DeactivateStealth();
            }
            else if (Dist >= RevealDistance && !bIsStealthed)
            {
                ActivateStealth();
            }
        }
    }
}

void UStealthComponent::ActivateStealth()
{
    SetVisibility(false);
    bIsStealthed = true;
}

void UStealthComponent::DeactivateStealth()
{
    SetVisibility(true);
    bIsStealthed = false;
}

void UStealthComponent::SetVisibility(bool bVisible)
{
    if (AActor* Owner = GetOwner())
    {
        USkeletalMeshComponent* SkeletalMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
        if (SkeletalMesh)
        {
            SkeletalMesh->SetVisibility(bVisible, true);
            SkeletalMesh->SetHiddenInGame(!bVisible, true);
        }
    }
}

AActor* UStealthComponent::FindNearestPlayer()
{
    AActor* Owner = GetOwner();
    if (!Owner) return nullptr;

    float MinDist = FLT_MAX;
    AActor* Nearest = nullptr;

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            APawn* PlayerPawn = PC->GetPawn();
            if (PlayerPawn)
            {
                float Dist = FVector::Dist(PlayerPawn->GetActorLocation(), Owner->GetActorLocation());
                if (Dist < MinDist)
                {
                    MinDist = Dist;
                    Nearest = PlayerPawn;
                }
            }
        }
    }

    return Nearest;
}

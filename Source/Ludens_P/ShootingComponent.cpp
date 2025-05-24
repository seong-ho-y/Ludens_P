// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootingComponent.h"

// Sets default values for this component's properties
UShootingComponent::UShootingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UShootingComponent::FireAt(AActor* Target)
{
	UE_LOG(LogTemp, Log, TEXT("FireAt in ShootingComponent"));
	if (!bCanFire || !Target || !GetOwner()->HasAuthority()) return;

	FVector MuzzleLoc = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * MuzzleOffset;
	FRotator MuzzleRot = (Target->GetActorLocation() - MuzzleLoc).Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();
	SpawnParams.Instigator = Cast<APawn>(GetOwner());
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLoc, MuzzleRot, SpawnParams);

	bCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(FireCooldownHandle, this, &UShootingComponent::ResetFireCooldown, FireCooldown, false);
}

void UShootingComponent::ResetFireCooldown()
{
	bCanFire = true;
}

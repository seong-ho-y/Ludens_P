// Fill out your copyright notice in the Description page of Project Settings.


#include "StealthComponent.h"

#include "GameFramework/Character.h"

// Sets default values for this component's properties
UStealthComponent::UStealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	
	// ...
}

void UStealthComponent::EnterStealth()
{
	TargetOpacity = StealthOpacity;
}

void UStealthComponent::ExitStealth()
{
	TargetOpacity = 1.0f;
}


// Called when the game starts
void UStealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	CurrentOpacity =1.0f;
	TargetOpacity = 1.0f;

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (OwnerCharacter)
	{
		USkeletalMeshComponent* OwnerMesh = OwnerCharacter->GetMesh();
		if (OwnerMesh)
		{
			for (int32 i = 0; i < OwnerMesh->GetNumMaterials(); i++)
			{
				UMaterialInstanceDynamic* MID = OwnerMesh->CreateAndSetMaterialInstanceDynamic(i);
				DynamicMaterials.Add(MID);
			}
		}
	}
}


// Called every frame
void UStealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	CurrentOpacity = FMath::FInterpTo(CurrentOpacity, TargetOpacity, DeltaTime, ChangeSpeed);
	for (UMaterialInstanceDynamic* MID : DynamicMaterials)
	{
		if (MID)
		{
			MID->SetScalarParameterValue(OpacityParamName, CurrentOpacity);
		}
	}
}


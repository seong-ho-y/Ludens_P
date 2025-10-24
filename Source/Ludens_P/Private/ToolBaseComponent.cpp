// Fill out your copyright notice in the Description page of Project Settings.


#include "ToolBaseComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UToolBaseComponent::UToolBaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	CooldownTime = 3.0f;
	bIsOnCooldown = false;
}


void UToolBaseComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UToolBaseComponent::PlayToolSound(APawn* InstigatorPawn)
{
	if (!UseSound) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FVector Location = FVector::ZeroVector;

	// Pawn이 존재하면 위치를 그쪽으로 설정
	if (InstigatorPawn)
	{
		Location = InstigatorPawn->GetActorLocation();
	}
	else if (AActor* Owner = GetOwner())
	{
		Location = Owner->GetActorLocation();
	}

	UGameplayStatics::PlaySoundAtLocation(World, UseSound, Location);
}

void UToolBaseComponent::StartCooldown()
{
	bIsOnCooldown = true;
	GetWorld()->GetTimerManager().SetTimer(
		CooldownTimerHandle,
		this,
		&UToolBaseComponent::EndCooldown,
		CooldownTime,
		false
	);
}

void UToolBaseComponent::EndCooldown()
{
	bIsOnCooldown = false;
}

bool UToolBaseComponent::CanUseTool_Implementation() const
{
	return !bIsOnCooldown;
}

void UToolBaseComponent::Interact_Implementation(APawn* InstigatorPawn)
{
	if (bIsOnCooldown)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is on cooldown!"), *GetName());
		return;
	}

	PerformToolAction(InstigatorPawn);
	StartCooldown();
}

void UToolBaseComponent::PerformToolAction(APawn* InstigatorPawn)
{
	// 자식 클래스에서 구현할 부분
}
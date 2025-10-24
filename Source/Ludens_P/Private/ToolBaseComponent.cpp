// Fill out your copyright notice in the Description page of Project Settings.


#include "ToolBaseComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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
	SetIsReplicated(true);
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

void UToolBaseComponent::OnRep_CooldownPercent()
{
	OnCooldownUpdated.Broadcast(CooldownPercent);
}

void UToolBaseComponent::Server_StartCooldown_Implementation()
{
	if (bIsOnCooldown) return;
	bIsOnCooldown = true;
	ElapsedTime = 0.f;
	CooldownPercent = 1.f;

	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, [this]()
	{
		if (!bIsOnCooldown) return;

		ElapsedTime += 0.1f;
		CooldownPercent = FMath::Clamp(1.f - (ElapsedTime / CooldownTime), 0.f, 1.f);

		if (ElapsedTime >= CooldownTime)
		{
			bIsOnCooldown = false;
			CooldownPercent = 0.f;
			GetWorld()->GetTimerManager().ClearTimer(CooldownTimerHandle);
		}

		// Replication 업데이트 (변경 감지)
		MarkPackageDirty();

	}, 0.1f, true);
}

void UToolBaseComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UToolBaseComponent, CooldownPercent);
	DOREPLIFETIME(UToolBaseComponent, bIsOnCooldown);
}
void UToolBaseComponent::StartCooldown()
{
	bIsOnCooldown = true;
	CooldownElapsed = 0.f;
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
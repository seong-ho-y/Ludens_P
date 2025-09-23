// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosionComponent.h"
#include "DeathHandlerInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values for this component's properties
UExplosionComponent::UExplosionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UExplosionComponent::Explode()
{
	AActor* MyOwner = GetOwner();
	if (MyOwner == nullptr) return;

	if (ExplosionEffect != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Explode Effect Spawn"));
		FVector Location = MyOwner->GetActorLocation();
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffect,
			Location,
			FRotator(0, 0, 0),
			FVector(1.0f),
			true,
			true
			);
	}
	if (ExplosionSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, MyOwner->GetActorLocation());
	}
	if (MyOwner->Implements<UDeathHandlerInterface>())
	{
		IDeathHandlerInterface::Execute_HandleDeath(MyOwner);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No DeathHandlerInterface Implemented!!"));
		MyOwner->Destroy();
	}
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(MyOwner);

	UGameplayStatics::ApplyRadialDamage(
        this,
        ExplosionDamage,
        MyOwner->GetActorLocation(),
        ExplosionRadius,
        UDamageType::StaticClass(),
        IgnoredActors,
        MyOwner, // 피해를 입힌 주체
        MyOwner->GetInstigatorController(), // 주체의 컨트롤러
        true
    );

	//MyOwner->(); 이제 여기서 EnemyBase의 HandleDied를 다루어야함
	//이것을 위해서 Interface 작업을 해주어야함
}


// Called when the game starts 
void UExplosionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UExplosionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


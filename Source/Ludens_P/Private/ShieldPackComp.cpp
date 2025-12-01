// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldPackComp.h"

#include "PlayerStateComponent.h"
#include "PlayerState_Real.h"
#include "Ludens_P/Ludens_PCharacter.h"

// Sets default values for this component's properties
UShieldPackComp::UShieldPackComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UShieldPackComp::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UShieldPackComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UShieldPackComp::PerformToolAction(APawn* InstigatorPawn)
{
	Super::PerformToolAction(InstigatorPawn);
	Server_GetShield();
}


void UShieldPackComp::Server_GetShield_Implementation()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	APlayerState_Real* PSR = Cast<ALudens_PCharacter>(Owner)->PSR;
	UPlayerStateComponent* PSC = Owner->GetComponentByClass<UPlayerStateComponent>();
	if (!PSR || !PSC) return;

	UE_LOG(LogTemp, Warning, TEXT("GetShield Called"))
	// 버프 중복 방지
	if (GetWorld()->GetTimerManager().IsTimerActive(ShieldDurationHandle))
	{
		GetWorld()->GetTimerManager().ClearTimer(ShieldDurationHandle);
	}

	// 최초 버프 시점이라면 원본 저장
	if (!bIsShieldBuffActive)
	{
		OriginalMaxShield = PSR->MaxShield;
		bIsShieldBuffActive = true;
	}

	SpawnShieldVFX();
	// 쉴드 증가
	PSR->MaxShield += ShieldAmount;
	PSC->EnableShieldRegen();

	// 일정 시간 후 복귀
	GetWorld()->GetTimerManager().SetTimer(
		ShieldDurationHandle,
		this,
		&UShieldPackComp::RevertShield,
		ShieldDuration,
		false
	);

}
void UShieldPackComp::RevertShield()
{
	AActor* Owner = GetOwner();
	if (!Owner) return;

	APlayerState_Real* PSR = Cast<ALudens_PCharacter>(Owner)->PSR;
	UPlayerStateComponent* PSC = Owner->GetComponentByClass<UPlayerStateComponent>();
	if (!PSR || !PSC) return;
	
	PSR->MaxShield = OriginalMaxShield;

	UE_LOG(LogTemp, Log, TEXT("Shield reverted to %.1f"), PSR->MaxShield);
}

void UShieldPackComp::SpawnShieldVFX_Implementation()
{
	if (ShiledVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(
    ShiledVFX,                     // Niagara 시스템
    GetOwner()->GetRootComponent(),       // 부모 컴포넌트 (플레이어 루트)
    NAME_None,                            // 소켓 이름 (없으면 루트 기준)
    FVector::ZeroVector,                  // 오프셋
    FRotator::ZeroRotator,                // 회전
    EAttachLocation::KeepRelativeOffset,  // 상대 위치 유지
    true                                  // 액터가 없어지면 자동 파괴
		);
	}
	if (ShieldSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ShieldSound, GetOwner()->GetActorLocation());
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserComponent.h"

// Sets default values for this component's properties
ULaserComponent::ULaserComponent()
{
	// 컴포넌트 복제 활성화
	SetIsReplicatedByDefault(true);

	// ✨ 1. LaserFX 라는 이름의 NiagaraComponent를 '자식 오브젝트'로 생성합니다.
	LaserComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LaserFX"));

	// ✨ 2. 생성된 NiagaraComponent를 이 LaserComponent에 부착(Attach)합니다.
	//    (이 컴포넌트가 액터에 부착되면, LaserFX도 함께 따라다닙니다)
	LaserComp->SetupAttachment(this); 
    
	// ✨ 3. 처음에는 꺼진 상태로 시작하도록 설정합니다.
	LaserComp->bAutoActivate = false;
}


// Called when the game starts
void ULaserComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
void ULaserComponent::TurnOn()
{
	if (!GetOwner()->HasAuthority()) return;
	Multicast_SetLaserActive(true);
}

void ULaserComponent::TurnOff()
{
	if (!GetOwner()->HasAuthority()) return;
	Multicast_SetLaserActive(false);
}

void ULaserComponent::Multicast_SetLaserActive_Implementation(bool bLaserActive)
{
	if (!LaserComp) return;
	LaserComp->SetActive(bLaserActive);
}

// Called every frame
void ULaserComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


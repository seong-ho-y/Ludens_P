// Fill out your copyright notice in the Description page of Project Settings.


#include "JellooComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/WidgetComponent.h"
#include "Ludens_P/EnemyHealthBarBase.h"

// Sets default values for this component's properties
UJellooComponent::UJellooComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);
	/*MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	
	//MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetEnableGravity(true);
	MeshComponent->SetIsReplicated(true);*/
}


// Called when the game starts
void UJellooComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentJellooHP = MaxJellooHP;

	HealthBarWidget = GetOwner()->FindComponentByClass<UWidgetComponent>();

	if (HealthBarWidget)
	{
		UUserWidget* Widget = HealthBarWidget->GetUserWidgetObject();
		HealthBar = Cast<UEnemyHealthBarBase>(Widget); // 실제 UI 클래스 캐스팅
	}
}


// Called every frame
void UJellooComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UJellooComponent::OnRep_JellooHP()
{
	// CurrentJellooHP 값이 바뀌었을 때 처리 UI 처리 필요
	if (HealthBar)
	{
		float Percent = static_cast<float>(CurrentJellooHP) / static_cast<float>(MaxJellooHP);
		HealthBar->UpdateHealthBar(Percent);
	}
	
	// 나이아가라 이펙트가 있다면 액터 위치에 스폰
	if (JellooHit) // UNiagaraSystem* 타입 멤버 변수로 선언해야 함
	{
		FVector Location = GetOwner()->GetActorLocation();
		FRotator Rotation = GetOwner()->GetActorRotation();
		
		MulticastSpawnEffect(JellooHit,Location, Rotation);
	}
}


void UJellooComponent::JellooTakeDamage(float Amount)
{
	if (!GetOwner()->HasAuthority()) return;
	if (bIsDead) return;
	CurrentJellooHP -= Amount;
	UE_LOG(LogTemp,Warning,TEXT("Jelloo Absorbed : %hd"),CurrentJellooHP);
	OnRep_JellooHP();

	if (CurrentJellooHP <= 0.f)
	{
		DestroyJelloo();
	}
}

void UJellooComponent::DestroyJelloo()
{
	// 반드시 서버에서만 로직 실행
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		Server_DestroyJelloo();
		return;
	}
	bIsDead = true;

	// 나이아가라 이펙트가 있다면 액터 위치에 스폰
	if (JellooDestroy) // UNiagaraSystem* 타입 멤버 변수로 선언해야 함
	{
		FVector Location = GetOwner()->GetActorLocation();
		FRotator Rotation = GetOwner()->GetActorRotation();
		
		MulticastSpawnEffect(JellooDestroy,Location, Rotation);
	}
	
	AActor* OwnerChar = Cast<AActor>(GetOwner());
	if (!OwnerChar)
	{
		UE_LOG(LogTemp, Error, TEXT("DestroyJelloo(): 소유자 액터가 없습니다."));
	}
	
	OwnerChar->Destroy();
}

void UJellooComponent::Server_DestroyJelloo_Implementation()
{
	DestroyJelloo();
}

void UJellooComponent::MulticastSpawnEffect_Implementation(UNiagaraSystem* NiagaraEffect, FVector Location, FRotator Rotation)
{
	// 서버에서 젤루 파괴시 나이아가라 재생
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NiagaraEffect, Location, Rotation);
}

void UJellooComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UJellooComponent, MaxJellooHP);
	DOREPLIFETIME(UJellooComponent, CurrentJellooHP);
}

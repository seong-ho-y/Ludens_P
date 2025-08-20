// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

ADoor::ADoor()
{
    bReplicates = true;
    SetReplicateMovement(true); // 문이 움직이는 경우

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootComponent);
    DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));    // 초기 충돌 설정

    bIsOpen = false;
}

void ADoor::BeginPlay()
{
    Super::BeginPlay();

    ApplyDoorState();   // 초기 상태 반영
}

void ADoor::Open()
{
    if (HasAuthority() && !bIsOpen)
    {
        UE_LOG(LogTemp, Warning, TEXT("[Server] %s Open()"), *GetName());
        bIsOpen = true;     // 서버에서 값만 바꾸면 클라에서 OnRep 호출됨
        ApplyDoorState();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] %s Open() IGNORED (Auth=%d, bIsOpen=%d)"),
            HasAuthority() ? TEXT("Server") : TEXT("Client"), *GetName(), HasAuthority(), bIsOpen);
    }
}

void ADoor::Close()
{
    if (HasAuthority() && bIsOpen)
    {
        bIsOpen = false;
        ApplyDoorState();

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Orange, TEXT("Door Closed"));
        }
    }
}

void ADoor::ApplyDoorState()
{
    if (!DoorMesh) return;

    // 가시성
    DoorMesh->SetVisibility(!bIsOpen);
    DoorMesh->SetHiddenInGame(bIsOpen);
    
    // 충돌: 액터 + 메시 둘 다 확실히 끄고/켜기
    SetActorEnableCollision(!bIsOpen);
    DoorMesh->SetCollisionEnabled(bIsOpen ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics);
}

void ADoor::OnRep_DoorStateChanged()
{
    UE_LOG(LogTemp, Warning, TEXT("[Client] %s OnRep_DoorStateChanged -> %s"),
        *GetName(), bIsOpen ? TEXT("OPEN") : TEXT("CLOSED"));
    ApplyDoorState();
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADoor, bIsOpen);
}

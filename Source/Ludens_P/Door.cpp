// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
<<<<<<< Updated upstream
=======
#include "Net/UnrealNetwork.h"
>>>>>>> Stashed changes

ADoor::ADoor()
{
    bReplicates = true;
    SetReplicateMovement(true); // 문이 움직이는 경우

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootComponent);
<<<<<<< Updated upstream

    // 초기 충돌 설정
    DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));
=======
    DoorMesh->SetCollisionProfileName(TEXT("BlockAll")); // 초기 충돌 설정

    bIsOpen = false;
>>>>>>> Stashed changes
}

void ADoor::BeginPlay()
{
    Super::BeginPlay();
<<<<<<< Updated upstream
=======
    ApplyDoorState(); // 초기 상태 반영
>>>>>>> Stashed changes
}

void ADoor::Open()
{
<<<<<<< Updated upstream
    if (!bIsOpen && HasAuthority())
    {
        bIsOpen = true;

        MulticastOpen();  // 모든 클라이언트에 문 숨기기 전파

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Door Opened"));
        }
=======
    if (HasAuthority() && !bIsOpen)
    {
        bIsOpen = true; // 서버에서 값만 바꾸면 클라에서 OnRep 호출됨
        ApplyDoorState();

        UE_LOG(LogTemp, Log, TEXT("Door opened (Server)"));
>>>>>>> Stashed changes
    }
}

void ADoor::Close()
{
<<<<<<< Updated upstream
    if (bIsOpen)
    {
        bIsOpen = false;

        if (HasAuthority())
        {
            MulticastClose();  // 모든 클라이언트에 문 생성 전파

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Door Closed"));
            }
        }
    }
}

void ADoor::MulticastOpen_Implementation()
{
    SetActorHiddenInGame(true);     // 안 보이게
    SetActorEnableCollision(false); // 충돌 꺼짐
}

void ADoor::MulticastClose_Implementation()
{
    SetActorHiddenInGame(false);    // 보이게
    SetActorEnableCollision(true);  // 충돌 켜짐
=======
    if (HasAuthority() && bIsOpen)
    {
        bIsOpen = false;
        ApplyDoorState();

        UE_LOG(LogTemp, Log, TEXT("Door closed (Server)"));
    }
}

void ADoor::ApplyDoorState()
{
    if (!DoorMesh) return;

    DoorMesh->SetVisibility(!bIsOpen);
    DoorMesh->SetHiddenInGame(bIsOpen);
    SetActorEnableCollision(!bIsOpen);

    FString Who = HasAuthority() ? TEXT("Server") : TEXT("Client");
    FString State = bIsOpen ? TEXT("Opened") : TEXT("Closed");

    UE_LOG(LogTemp, Log, TEXT("[%s] Door %s via ApplyDoorState()"), *Who, *State);
}

void ADoor::OnRep_DoorStateChanged()
{
    ApplyDoorState();

    UE_LOG(LogTemp, Log, TEXT("OnRep_DoorStateChanged() called on Client"));
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADoor, bIsOpen);
>>>>>>> Stashed changes
}
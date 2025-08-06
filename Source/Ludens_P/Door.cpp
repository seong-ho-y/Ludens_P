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
    SetReplicateMovement(true); // ���� �����̴� ���

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootComponent);
<<<<<<< Updated upstream

    // �ʱ� �浹 ����
    DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));
=======
    DoorMesh->SetCollisionProfileName(TEXT("BlockAll")); // �ʱ� �浹 ����

    bIsOpen = false;
>>>>>>> Stashed changes
}

void ADoor::BeginPlay()
{
    Super::BeginPlay();
<<<<<<< Updated upstream
=======
    ApplyDoorState(); // �ʱ� ���� �ݿ�
>>>>>>> Stashed changes
}

void ADoor::Open()
{
<<<<<<< Updated upstream
    if (!bIsOpen && HasAuthority())
    {
        bIsOpen = true;

        MulticastOpen();  // ��� Ŭ���̾�Ʈ�� �� ����� ����

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Door Opened"));
        }
=======
    if (HasAuthority() && !bIsOpen)
    {
        bIsOpen = true; // �������� ���� �ٲٸ� Ŭ�󿡼� OnRep ȣ���
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
            MulticastClose();  // ��� Ŭ���̾�Ʈ�� �� ���� ����

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, TEXT("Door Closed"));
            }
        }
    }
}

void ADoor::MulticastOpen_Implementation()
{
    SetActorHiddenInGame(true);     // �� ���̰�
    SetActorEnableCollision(false); // �浹 ����
}

void ADoor::MulticastClose_Implementation()
{
    SetActorHiddenInGame(false);    // ���̰�
    SetActorEnableCollision(true);  // �浹 ����
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
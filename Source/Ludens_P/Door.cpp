// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

ADoor::ADoor()
{
    bReplicates = true;
    SetReplicateMovement(true); // ���� �����̴� ���

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootComponent);

    // �ʱ� �浹 ����
    DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));
}

void ADoor::BeginPlay()
{
    Super::BeginPlay();
}

void ADoor::Open()
{
    if (!bIsOpen && HasAuthority())
    {
        bIsOpen = true;

        MulticastOpen();  // ��� Ŭ���̾�Ʈ�� �� ����� ����

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Door Opened"));
        }
    }
}

void ADoor::Close()
{
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
}
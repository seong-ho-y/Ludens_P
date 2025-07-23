// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

ADoor::ADoor()
{
    bReplicates = true;
    SetReplicateMovement(true); // 문이 움직이는 경우

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootComponent);

    // 초기 충돌 설정
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

        MulticastOpen();  // 모든 클라이언트에 문 숨기기 전파

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
}
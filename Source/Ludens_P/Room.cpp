// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"
#include "Door.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

// Sets default values
ARoom::ARoom()
{

}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
	Super::BeginPlay();
	
    // 10초 후 자동 클리어
    GetWorld()->GetTimerManager().SetTimer(ClearTimerHandle, this, &ARoom::SimulateRoomClear, 10.f, false);

    if (TriggerVolume)
    {
        TriggerVolume->OnActorBeginOverlap.AddDynamic(this, &ARoom::OnTriggerEnter);
        TriggerVolume->OnActorEndOverlap.AddDynamic(this, &ARoom::OnTriggerExit);
    }
}

void ARoom::SimulateRoomClear()
{
    bIsCleared = true;

    if (ExitDoor)
    {
        ExitDoor->SetActorHiddenInGame(true);
        ExitDoor->SetActorEnableCollision(false);
    }

    SpawnNextRoom(); // 새로운 방 생성
}

void ARoom::SpawnNextRoom()
{
    FVector NextLocation = GetActorLocation() + FVector(2000, 0, 0);

    ARoom* NewRoom = GetWorld()->SpawnActor<ARoom>(RoomClass, NextLocation, FRotator::ZeroRotator);
}

void ARoom::OnTriggerEnter(AActor* OverlappedActor, AActor* OtherActor)
{
    APlayerController* PC = Cast<APlayerController>(OtherActor->GetInstigatorController());
    if (PC && !OverlappedPlayers.Contains(PC))
    {
        OverlappedPlayers.Add(PC);

        if (OverlappedPlayers.Num() >= RequiredPlayersToProceed)
        {
            if (ExitDoor)
            {
                ExitDoor->SetActorHiddenInGame(false);
                ExitDoor->SetActorEnableCollision(true);
            }

            RemoveCurrentRoom(); // 이전 방 삭제
        }
    }
}

void ARoom::OnTriggerExit(AActor* OverlappedActor, AActor* OtherActor)
{
    APlayerController* PC = Cast<APlayerController>(OtherActor->GetInstigatorController());
    if (PC)
    {
        OverlappedPlayers.Remove(PC);
    }
}

void ARoom::RemoveCurrentRoom()
{
    // 일정 시간 후 자신을 제거
    SetLifeSpan(2.f); // 2초 뒤 Destroy()
}
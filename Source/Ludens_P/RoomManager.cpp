// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomManager.h"
#include "Room.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARoomManager::ARoomManager()
{
 	
}

// Called when the game starts or when spawned
void ARoomManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARoomManager::NotifyRoomCleared(ARoom* ClearedRoom)
{
    if (!ClearedRoom) return;

    // 다음 방 위치 계산
    FVector NextRoomLocation = ClearedRoom->GetActorLocation() + FVector(2000, 0, 0);
    ARoom* NewRoom = GetWorld()->SpawnActor<ARoom>(RoomClass, NextRoomLocation, FRotator::ZeroRotator);

    RoomInstances.Add(NewRoom);
    RoomCount++;
}
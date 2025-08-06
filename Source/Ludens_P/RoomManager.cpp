// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomManager.h"
#include "Room.h"
#include "Door.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Engine/Engine.h"

// Sets default values
ARoomManager::ARoomManager()
{
    bReplicates = true;
}

// Called when the game starts or when spawned
void ARoomManager::BeginPlay()
{
	Super::BeginPlay();

    if (HasAuthority())
    {
        GenerateRooms(); // 방 생성
        StartNextRoom(); // 첫 방
    }
}

void ARoomManager::GenerateRooms()
{
    FVector StartLocation = GetActorLocation();

    for (int32 i = 0; i < NumRoomsToSpawn; ++i)
    {
        // 방 종류 무작위 선택
        int32 RandIndex = FMath::RandRange(0, RoomTypes.Num() - 1);
        TSubclassOf<ARoom> SelectedRoom = RoomTypes[RandIndex];

        FVector SpawnLocation = StartLocation + FVector(i * RoomSpacing, 0.f, 0.f);
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;

        ARoom* NewRoom = GetWorld()->SpawnActor<ARoom>(SelectedRoom, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        if (NewRoom)
        {
            NewRoom->SetRoomIndex(i);     // 방의 인덱스 설정
            NewRoom->SetManager(this);    // Room → Manager 역참조
            SpawnedRooms.Add(NewRoom);

            if (GEngine)
            {
                FString Msg = FString::Printf(TEXT("Room %d Created (%s)"), i, *SelectedRoom->GetName());
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Msg);
            }
        }
    }
}

void ARoomManager::StartNextRoom()
{
    if (SpawnedRooms.IsValidIndex(CurrentRoomIndex))
    {
        ARoom* RoomToStart = SpawnedRooms[CurrentRoomIndex];
        if (RoomToStart)
        {
            RoomToStart->StartRoom();
        }
    }
    else
    {
        // 게임 종료 처리
        if (GEngine && HasAuthority())
        {
            GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Red, TEXT("All Room Cleared!"));
        }
    }
}

void ARoomManager::NotifyRoomCleared(int32 RoomIndex)
{
    if (RoomIndex == CurrentRoomIndex)
    {
        if (SpawnedRooms.IsValidIndex(RoomIndex))
        {
            ARoom* CurrentRoom = SpawnedRooms[RoomIndex];
            ARoom* NextRoom = SpawnedRooms.IsValidIndex(RoomIndex + 1) ? SpawnedRooms[RoomIndex + 1] : nullptr;

            // 현재 방의 출구 문 열기
            if (CurrentRoom && CurrentRoom->ExitDoor)
            {
                CurrentRoom->ExitDoor->Open();
            }

            // 다음 방의 입구 문 열기
            if (NextRoom && NextRoom->EntryDoor)
            {
                NextRoom->EntryDoor->Open();
            }
        }

<<<<<<< Updated upstream
        CurrentRoomIndex++;
=======
        // 이전 방 비활성화
        SpawnedRooms[CurrentRoomIndex]->EntryTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        CurrentRoomIndex++;

        if (SpawnedRooms.IsValidIndex(CurrentRoomIndex))
        {
            SpawnedRooms[CurrentRoomIndex]->EntryTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        }
>>>>>>> Stashed changes
    }
}

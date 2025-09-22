// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomManager.h"
#include "Room.h"
#include "Elevator.h"
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
        GenerateRooms();

        if (StartElevator)
        {
            // Elevator가 “3인 집결” 시 브로드캐스트할 델리게이트에 바인딩 (아래 2) 참고)
            StartElevator->OnAllPlayersReady.AddDynamic(this, &ARoomManager::OpenFirstRoomEntryDoor);
        }
        else
        {
            if (GEngine)
            {
                FString Msg = FString::Printf(TEXT("There is no StartElevator!"));
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Msg);
            }
        }
    }
}

void ARoomManager::GenerateRooms()
{
    FVector StartLocation = GetActorLocation();
    float z = 200.f;

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

void ARoomManager::OpenFirstRoomEntryDoor()
{
    // 첫 방 인덱스는 0
    if (StartElevator->ExitDoor)
    {
        StartElevator->EntryDoor->Open();
        StartElevator->ExitDoor->Open();
    }

    if (SpawnedRooms.IsValidIndex(0) && SpawnedRooms[0]->EntryDoor)
    {
        SpawnedRooms[0]->EntryDoor->Open();
    }
    // 실제 "첫 방 시작"은 첫 방 EntryTrigger에서 3인 감지되면 자동 진행됨(기존 로직 유지)
}

void ARoomManager::StartNextRoom()
{
    if (!HasAuthority()) return;

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
        if (GEngine)
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

        // 마지막 방이면 EndElevator 입장 허용(선택)
        const bool bIsLastRoom = (RoomIndex == NumRoomsToSpawn - 1);
        if (bIsLastRoom && EndElevator && EndElevator->EntryDoor)
        {
            EndElevator->EntryDoor->Open();
        }

        // 이전 방 비활성화
        SpawnedRooms[CurrentRoomIndex]->EntryTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        CurrentRoomIndex++;

        if (SpawnedRooms.IsValidIndex(CurrentRoomIndex))
        {
            SpawnedRooms[CurrentRoomIndex]->EntryTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        }
    }
}

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
        GenerateRooms(); // �� ����
        StartNextRoom(); // ù ��
    }
}

void ARoomManager::GenerateRooms()
{
    FVector StartLocation = GetActorLocation();

    for (int32 i = 0; i < NumRoomsToSpawn; ++i)
    {
        // �� ���� ������ ����
        int32 RandIndex = FMath::RandRange(0, RoomTypes.Num() - 1);
        TSubclassOf<ARoom> SelectedRoom = RoomTypes[RandIndex];

        FVector SpawnLocation = StartLocation + FVector(i * RoomSpacing, 0.f, 0.f);
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;

        ARoom* NewRoom = GetWorld()->SpawnActor<ARoom>(SelectedRoom, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        if (NewRoom)
        {
            NewRoom->SetRoomIndex(i);     // ���� �ε��� ����
            NewRoom->SetManager(this);    // Room �� Manager ������
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
        // ���� ���� ó��
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

            // ���� ���� �ⱸ �� ����
            if (CurrentRoom && CurrentRoom->ExitDoor)
            {
                CurrentRoom->ExitDoor->Open();
            }

            // ���� ���� �Ա� �� ����
            if (NextRoom && NextRoom->EntryDoor)
            {
                NextRoom->EntryDoor->Open();
            }
        }

<<<<<<< Updated upstream
        CurrentRoomIndex++;
=======
        // ���� �� ��Ȱ��ȭ
        SpawnedRooms[CurrentRoomIndex]->EntryTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        CurrentRoomIndex++;

        if (SpawnedRooms.IsValidIndex(CurrentRoomIndex))
        {
            SpawnedRooms[CurrentRoomIndex]->EntryTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        }
>>>>>>> Stashed changes
    }
}

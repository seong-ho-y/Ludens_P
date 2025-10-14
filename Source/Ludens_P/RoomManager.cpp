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
        if (!Start || !End)
        {
            UE_LOG(LogTemp, Error, TEXT("[RoomManager] Start/End Elevator class not set!"));
            return; // 스폰 시도 자체를 중단
        }

        FActorSpawnParameters Params;
        Params.Owner = this;
        Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        StartElevator = GetWorld()->SpawnActor<AElevator>(Start, GetActorLocation(), FRotator::ZeroRotator, Params);
        EndElevator = GetWorld()->SpawnActor<AElevator>(End, GetActorLocation(), FRotator::ZeroRotator, Params);

        if (!StartElevator || !EndElevator)
        {
            UE_LOG(LogTemp, Error, TEXT("[RoomManager] Failed to spawn elevators"));
            return;
        }

        GenerateRooms();   // 방 생성
        LayoutChain();     // 체인 배치

        // 엘리베이터 신호 바인딩
        StartElevator->OnAllPlayersReady.AddDynamic(this, &ARoomManager::HandleStartElevatorReady);
        EndElevator->OnAllPlayersReady.AddDynamic(this, &ARoomManager::HandleEndElevatorReady);
    }
}

void ARoomManager::GenerateRooms()
{
    SpawnedRooms.Reset();

    if (RoomTypes.Num() == 0) return;

    const FRotator Rot = FRotator::ZeroRotator;
    FActorSpawnParameters Params; Params.Owner = this;

    for (int32 i = 0; i < NumRoomsToSpawn; ++i)
    {
        const int32 RandIndex = FMath::RandRange(0, RoomTypes.Num() - 1);
        TSubclassOf<ARoom> Chosen = RoomTypes[RandIndex];

        // 일단 대략적인 위치(매니저 위치)로 스폰 → 나중에 LayoutChain()에서 재배치
        ARoom* NewRoom = GetWorld()->SpawnActor<ARoom>(Chosen, GetActorLocation(), Rot, Params);
        if (NewRoom)
        {
            NewRoom->SetRoomIndex(i);
            NewRoom->SetManager(this);
            SpawnedRooms.Add(NewRoom);

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
                    FString::Printf(TEXT("Room %d Created (%s)"), i, *Chosen->GetName()));
            }
        }
    }
}

void ARoomManager::OpenFirstRoomEntryDoor()
{
    // 첫 방 인덱스는 0
    if (StartElevator->ExitDoor)
    {
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
        if (ARoom* RoomToStart = SpawnedRooms[CurrentRoomIndex]) { RoomToStart->StartRoom(); }
    }
    else
    {
        // 게임 종료 처리
        if (GEngine) GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Red, TEXT("All Room Cleared!"));
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
            if (CurrentRoom && CurrentRoom->ExitDoor) { CurrentRoom->ExitDoor->Open(); }

            // 다음 방의 입구 문 열기
            if (NextRoom && NextRoom->EntryDoor) { NextRoom->EntryDoor->Open(); }
        }

        // 마지막 방이면 EndElevator 입장 허용(선택)
        const bool bIsLastRoom = (RoomIndex == NumRoomsToSpawn - 1);
        if (bIsLastRoom && EndElevator && EndElevator->EntryDoor) { EndElevator->EntryDoor->Open(); }

        // 이전 방 비활성화
        SpawnedRooms[CurrentRoomIndex]->EntryTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

        CurrentRoomIndex++;

        if (SpawnedRooms.IsValidIndex(CurrentRoomIndex))
        {
            SpawnedRooms[CurrentRoomIndex]->EntryTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        }
    }
}

void ARoomManager::LayoutChain()
{
    TArray<ARoom*> Chain;
    if (StartElevator) { Chain.Add(StartElevator); }
    Chain.Append(SpawnedRooms);
    if (EndElevator) { Chain.Add(EndElevator); }

    if (Chain.Num() == 0) return;

    const FVector Anchor = GetActorLocation();   // 체인의 시작 기준점
    float currX = Anchor.X;
    float baseZ = Anchor.Z;

    // 첫 노드: 기준점에 둡니다 (필요하면 Blueprint에서 StartElevator의 EntryDoorOffset.X가 음수 엣지로 향하도록 설정)
    {
        ARoom* First = Chain[0];
        const float halfW = First->GetHalfWidthX();
        // 첫 방의 중심을 기준점 + halfW 로 잡아 앞쪽(+)을 향해 이어 붙이기
        currX += halfW;
        First->SetActorLocation(FVector(currX, Anchor.Y, baseZ));
    }

    // 이후 노드: 이전 ExitDoor의 Y와 현재 EntryDoor의 Y가 일치하도록 Y를 보정
    for (int32 i = 1; i < Chain.Num(); ++i)
    {
        ARoom* Prev = Chain[i - 1];
        ARoom* Curr = Chain[i];

        const float prevHalf = Prev->GetHalfWidthX();
        const float currHalf = Curr->GetHalfWidthX();

        // X: (이전 반폭 + 간격 + 현재 반폭)만큼 전진
        currX += prevHalf + GapBetweenRooms + currHalf;

        // Y: “문-Y 정렬”
        const float desiredY = Prev->GetExitDoorWorldPos().Y - Curr->EntryDoorOffset.Y;

        Curr->SetActorLocation(FVector(currX, desiredY, baseZ));
    }
}

void ARoomManager::HandleStartElevatorReady()
{
    OpenFirstRoomEntryDoor();
}

void ARoomManager::HandleEndElevatorReady()
{
    TravelToNextStage();
}

void ARoomManager::TravelToNextStage()
{
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("[RoomManager] Travel requested on client - ignored."));
        return;
    }

    if (!NextStage.IsValid() && NextStage.ToSoftObjectPath().IsNull())
    {
        UE_LOG(LogTemp, Error, TEXT("[RoomManager] NextStage is not set!"));
        return;
    }

    const FString ObjPath = NextStage.ToSoftObjectPath().ToString(); // "/Game/Maps/Stage2.Stage2"
    FString PackagePath, ObjectName;
    if (!ObjPath.Split(TEXT("."), &PackagePath, &ObjectName))
    {
        UE_LOG(LogTemp, Error, TEXT("[RoomManager] Invalid map path: %s"), *ObjPath);
        return;
    }

    const bool bIsListenServer = (GetNetMode() != NM_DedicatedServer);
    const FString URL = bIsListenServer ? (PackagePath + TEXT("?listen")) : PackagePath;

    UE_LOG(LogTemp, Log, TEXT("[RoomManager] ServerTravel to: %s"), *URL);
    GetWorld()->ServerTravel(URL, /*bAbsolute*/ false);
}

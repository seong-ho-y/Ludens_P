// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomManager.h"
#include "Room.h"
#include "Elevator.h"
#include "Door.h"
#include "Algo/RandomShuffle.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

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

        // 새 스테이지 시작 ‘덜컥’ (BP_StartArrival) — 스테이지 이동 직후
        if (StartArrivalShake)
        {
            // 입력 전면 잠금 ON (대기 시작)
            LockAllPlayerInputs(true);

            // 아주 짧게 지연 후 흔들림 재생
            FTimerHandle TH;
            GetWorldTimerManager().SetTimer(TH, [this]()
                {
                    StartShake_AllPlayers(StartArrivalShake, StartArrivalShakeScale);

                    // 흔들림 길이에 맞춰 해제
                    FTimerHandle UnlockTH;
                    GetWorldTimerManager().SetTimer(
                        UnlockTH,
                        [this]() { LockAllPlayerInputs(false); },
                        StartArrivalInputLockSeconds,
                        false
                    );

                }, 1.f, false);
        }
    }
}

void ARoomManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    LockAllPlayerInputs(false);
    Super::EndPlay(EndPlayReason);
}

void ARoomManager::GenerateRooms()
{
    SpawnedRooms.Reset();
    if (RoomTypes.Num() == 0) return;

    // 1) 뽑기용 풀을 복사해서 섞는다
    TArray<TSubclassOf<ARoom>> Pool = RoomTypes;
    Algo::RandomShuffle(Pool);

    // 2) 타입 개수보다 더 많이 만들라고 하면, 가능한 만큼만
    const int32 CountToSpawn = FMath::Min(NumRoomsToSpawn, Pool.Num());

    const FRotator Rot = FRotator::ZeroRotator;
    FActorSpawnParameters Params; Params.Owner = this;

    for (int32 i = 0; i < CountToSpawn; ++i)
    {
        TSubclassOf<ARoom> Chosen = Pool[i];

        ARoom* NewRoom = GetWorld()->SpawnActor<ARoom>(Chosen, GetActorLocation(), Rot, Params);
        if (NewRoom)
        {
            NewRoom->SetRoomIndex(i);
            NewRoom->SetManager(this);
            SpawnedRooms.Add(NewRoom);

            UE_LOG(LogTemp, Warning, TEXT("Room %d Created (%s)"), i, *Chosen->GetName());
        }
    }

    // 요청 수가 더 많았던 경우엔 경고만 남겨두면 디버깅에 좋아요
    if (NumRoomsToSpawn > Pool.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("[RoomManager] Requested %d rooms, but only %d unique types available. Spawned %d."),
            NumRoomsToSpawn, Pool.Num(), CountToSpawn);
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
    if (StartElevator)
    { 
        Chain.Add(StartElevator);
        StartElevator->SetManager(this);
    }
    Chain.Append(SpawnedRooms);
    if (EndElevator)
    { 
        Chain.Add(EndElevator);
        EndElevator->SetManager(this);
    }

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


// 시작 엘리베이터 준비 완료(= 플레이어들이 첫 방 입구로 나갈 수 있는 순간)
void ARoomManager::HandleStartElevatorReady()
{
    // 첫 방 진입 가능하게 문 오픈
    OpenFirstRoomEntryDoor();
}


// 엔드 엘리베이터 모든 인원 탑승 완료
void ARoomManager::HandleEndElevatorReady()
{
    if (!HasAuthority()) return;

    // 0) 문 닫아 고정(기존 연출)
    if (EndElevator && EndElevator->EntryDoor) { EndElevator->EntryDoor->Close(); }

    const int32 LastRoomIdx = FMath::Max(0, NumRoomsToSpawn - 1);
    if (SpawnedRooms.IsValidIndex(LastRoomIdx))
    {
        if (SpawnedRooms[LastRoomIdx]->ExitDoor) { SpawnedRooms[LastRoomIdx]->ExitDoor->Close(); }
    }

    // "문이 완전히 닫힌 순간" = CloseTime 경과 시점에 흔들림 재생
    FTimerHandle TH_DepartShake;
    GetWorldTimerManager().SetTimer(TH_DepartShake, [this]()
        {
            if (DepartShake)
                StartShake_AllPlayers(DepartShake, DepartShakeScale);

            // 흔들림을 느낄 수 있도록 약간 더 기다렸다가(DelayBeforeTravel) 이동
            if (!GetWorldTimerManager().IsTimerActive(TravelTimer))
            {
                GetWorldTimerManager().SetTimer(
                    TravelTimer, this, &ARoomManager::TravelToNextStage,
                    DelayBeforeTravel, false
                );
            }
        }, EndElevator->EntryDoor->GetAnimDuration() + 0.8f, false);

    // 약간의 딜레이 후 스테이지 이동(기존 로직)
    if (!GetWorldTimerManager().IsTimerActive(TravelTimer))
    {
        GetWorldTimerManager().SetTimer(
            TravelTimer, this, &ARoomManager::TravelToNextStage,
            DelayBeforeTravel, false
        );
    }
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

void ARoomManager::StartShake_AllPlayers(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale) const
{
    if (!ShakeClass) return;
    UWorld* World = GetWorld();
    if (!World) return;

    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            PC->ClientStartCameraShake(ShakeClass, Scale);
        }
    }
}

void ARoomManager::LockAllPlayerInputs(bool bLock)
{
    if (UWorld* World = GetWorld())
    {
        for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
        {
            if (APlayerController* PC = It->Get())
            {
                if (bLock)
                {
                    // 1) 컨트롤러 입력 차단 + 시점/이동 무시
                    PC->DisableInput(PC);
                    PC->SetIgnoreMoveInput(true);
                    PC->SetIgnoreLookInput(true);

                    // 2) UIOnly로 전환 (게임 입력 라우팅 차단)
                    FInputModeUIOnly UIOnly;
                    PC->SetInputMode(UIOnly);
                    PC->bShowMouseCursor = true;

                    // 3) 클릭/마우스오버 이벤트도 차단 (좌클릭 눌림 자체를 막음)
                    PC->bEnableClickEvents = false;
                    PC->bEnableMouseOverEvents = false;

                    // 4) Pawn도 안전하게 차단
                    if (APawn* P = PC->GetPawn())
                    {
                        P->DisableInput(PC);
                    }
                }
                else
                {
                    // 1) Pawn/PC 모두 해제
                    if (APawn* P = PC->GetPawn())
                    {
                        P->EnableInput(PC);
                    }
                    PC->EnableInput(PC);
                    PC->SetIgnoreMoveInput(false);
                    PC->SetIgnoreLookInput(false);

                    // 2) 게임 모드로 복귀
                    FInputModeGameOnly GameOnly;
                    PC->SetInputMode(GameOnly);
                    PC->bShowMouseCursor = false;

                    // 3) 클릭/오버 이벤트 복구
                    PC->bEnableClickEvents = true;
                    PC->bEnableMouseOverEvents = true;
                }
            }
        }
    }
}

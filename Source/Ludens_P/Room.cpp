// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"
#include "Door.h"
#include "EnemySpawnPoint.h"
#include "RoomManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"    // 플레이어 판별용
#include "Ludens_PGameMode.h"
#include "RewardSystemComponent.h"

// Sets default values
ARoom::ARoom()
{
    bReplicates = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    EntryTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("EntryTrigger"));
    EntryTrigger->SetupAttachment(RootComponent);
    EntryTrigger->SetBoxExtent(FVector(200.f, 200.f, 200.f));
    EntryTrigger->SetCollisionProfileName(TEXT("Trigger"));
    EntryTrigger->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ARoom::BeginPlay()
{
    Super::BeginPlay();
    // StartRoom()은 Manager에서 직접 호출

    if (HasAuthority())
    {
        TArray<AActor*> AttachedActors;
        GetAttachedActors(AttachedActors);

        for (AActor* Actor : AttachedActors)
        {
            if (AEnemySpawnPoint* SpawnPoint = Cast<AEnemySpawnPoint>(Actor))
            {
                RoomSpawnPoints.Add(SpawnPoint);
            }
        }

        UE_LOG(LogTemp, Log, TEXT("[Room] %s found %d SpawnPoints"), *GetName(), RoomSpawnPoints.Num());
        if (EntryTrigger)
        {
            EntryTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARoom::OnEntryTriggerBegin);
        }

        if (DoorClass)
        {
            const FVector EntryLocation = GetEntryDoorWorldPos();
            const FVector ExitLocation = GetExitDoorWorldPos();
            const FRotator DoorRotation = FRotator::ZeroRotator;

            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            EntryDoor = GetWorld()->SpawnActor<ADoor>(DoorClass, EntryLocation, DoorRotation, SpawnParams);
            ExitDoor = GetWorld()->SpawnActor<ADoor>(DoorClass, ExitLocation, DoorRotation, SpawnParams);

            if (EntryDoor) { EntryDoor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform); }
            if (ExitDoor) { ExitDoor->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform); }
        }
    }
}

void ARoom::SetManager(ARoomManager* InManager)
{
    Manager = InManager;
}

void ARoom::SetRoomIndex(int32 Index)
{
    RoomIndex = Index;
}

void ARoom::StartRoom()
{
    bIsCleared = false;
    bClearPending = false;
    CancelPendingClear();

    if (HasAuthority())
    {
        if (ALudens_PGameMode* GM = GetWorld()->GetAuthGameMode<ALudens_PGameMode>())
        {
            GM->OnAllEnemiesKilled.RemoveDynamic(this, &ARoom::HandleAllEnemiesKilled);
            GM->OnAllEnemiesKilled.AddDynamic(this, &ARoom::HandleAllEnemiesKilled);

            // ✅ 기존 StartSpawningEnemies() 대신, 이 방의 포인트만 사용하도록 변경
            GM->StartSpawningEnemiesInRoom(this);

            // 방에 적이 하나도 없다면, 즉시가 아니라 “짧은 지연 후” 클리어
            if (GM->GetAliveEnemyCount() <= 0)
            {
                ScheduleClearWithDelay();
            }
        }
    }

    /*
    // 5초 후 자동 클리어
    GetWorld()->GetTimerManager().SetTimer(
        AutoClearTimer,
        this,
        &ARoom::AutoClear,
        5.f,
        false
    );
    */
}


void ARoom::HandleAllEnemiesKilled()
{
    if (!HasAuthority() || bIsCleared) return;

    // 즉시 클리어 대신 지연 예약
    ScheduleClearWithDelay();
}

void ARoom::ScheduleClearWithDelay()
{
    if (bClearPending || bIsCleared) return;   // 중복 예약 방지

    bClearPending = true;

    // 혹시 이전 타이머가 남아있다면 정리
    GetWorldTimerManager().ClearTimer(ClearDelayTimer);

    // 짧은 지연 후 실제 클리어
    GetWorldTimerManager().SetTimer(
        ClearDelayTimer,
        [this]()
        {
            // 타이머가 도는 사이에 설계 상 적이 다시 스폰될 가능성이 없다면 바로 클리어
            // (여기서 안전하게 GameMode 카운트 재확인도 가능)
            if (ALudens_PGameMode* GM = GetWorld()->GetAuthGameMode<ALudens_PGameMode>())
            {
                if (GM->GetAliveEnemyCount() > 0)
                {
                    // 만약 도중에 적이 다시 생겼다면 예약 취소
                    bClearPending = false;
                    return;
                }
            }
            DoClear();
        },
        ClearDelaySeconds,
        false
    );
}

void ARoom::CancelPendingClear()
{
    if (bClearPending)
    {
        GetWorldTimerManager().ClearTimer(ClearDelayTimer);
        bClearPending = false;
    }
}

void ARoom::DoClear()
{
    if (bIsCleared) return;

    bIsCleared = true;
    bClearPending = false;

    // 더 이상 이벤트 받을 필요 없음
    if (ALudens_PGameMode* GM = GetWorld()->GetAuthGameMode<ALudens_PGameMode>())
    {
        GM->OnAllEnemiesKilled.RemoveDynamic(this, &ARoom::HandleAllEnemiesKilled);
    }

    // 문 열기/다음 방 트리거 활성화 등은 Manager로 통지 (기존 AutoClear와 동일)
    if (Manager)
    {
        Manager->NotifyRoomCleared(RoomIndex);
    }

    // 모든 플레이어에게 보상 UI 띄우기
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = Cast<APlayerController>(*It))
        {
            if (ACharacter* PlayerChar = Cast<ACharacter>(PC->GetPawn()))
            {
                if (URewardSystemComponent* RewardComp = PlayerChar->FindComponentByClass<URewardSystemComponent>())
                {
                    RewardComp->Server_ShowRewardOptions();
                }
            }
        }
    }
}

void ARoom::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CancelPendingClear();

    if (HasAuthority())
    {
        if (ALudens_PGameMode* GM = GetWorld()->GetAuthGameMode<ALudens_PGameMode>())
        {
            GM->OnAllEnemiesKilled.RemoveDynamic(this, &ARoom::HandleAllEnemiesKilled);
        }
    }
    Super::EndPlay(EndPlayReason);
}

void ARoom::OnEntryTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor) return;

    ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor);
    if (!OverlappingCharacter) return;

    // 부모에서 승격한 EnteredPlayers 사용
    EnteredPlayers.Add(OverlappingCharacter);

    if (HasAuthority() && Manager)
    {
        if (EnteredPlayers.Num() == Manager->RequiredPlayers)
        {
            if (EntryDoor) EntryDoor->Close();      // 문 닫고
            Manager->StartNextRoom();  // 다음 방 시작

            DisableEntryTrigger(true);
        }
    }
}

void ARoom::DisableEntryTrigger(bool bDestroyComponent /*=true*/)
{
    if (!EntryTrigger) return;

    // 겹침/충돌 중단
    EntryTrigger->SetGenerateOverlapEvents(false);
    EntryTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 에디터/게임에서 보이지 않게(디버그 도형 등)
    EntryTrigger->SetHiddenInGame(true);
    EntryTrigger->SetVisibility(false, true);

    // 더 이상 필요 없다면 파괴
    if (bDestroyComponent)
    {
        EntryTrigger->DestroyComponent();
        EntryTrigger = nullptr;
    }
}

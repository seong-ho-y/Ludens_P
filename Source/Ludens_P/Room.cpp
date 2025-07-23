// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"
#include "Door.h"
#include "RoomManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Character.h" // 플레이어 판별용

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

    if (EntryTrigger)
    {
        EntryTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARoom::OnEntryTriggerBegin);
    }

    if (DoorClass)
    {
        // 문 위치 설정 (원하는 위치로 조정)
        FVector EntryLocation = GetActorLocation() + FVector(-1000.f, 0.f, 0.f);
        FVector ExitLocation = GetActorLocation() + FVector(1000.f, 0.f, 0.f);
        FRotator DoorRotation = FRotator::ZeroRotator;

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;

        // EntryDoor 생성
        EntryDoor = GetWorld()->SpawnActor<ADoor>(DoorClass, EntryLocation, DoorRotation, SpawnParams);

        // ExitDoor 생성
        ExitDoor = GetWorld()->SpawnActor<ADoor>(DoorClass, ExitLocation, DoorRotation, SpawnParams);
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

    if (GEngine && HasAuthority())
    {
        FString Msg = FString::Printf(TEXT("Room %d Start"), RoomIndex);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, Msg);
    }

    // 5초 후 자동 클리어
    GetWorld()->GetTimerManager().SetTimer(
        AutoClearTimer,
        this,
        &ARoom::AutoClear,
        5.f,
        false
    );
}

void ARoom::AutoClear()
{
    if (bIsCleared) return;
    bIsCleared = true;

    if (GEngine && HasAuthority())
    {
        FString Msg = FString::Printf(TEXT("Room %d Cleared"), RoomIndex);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, Msg);
    }

    if (Manager)
    {
        Manager->NotifyRoomCleared(RoomIndex);
    }
}

void ARoom::OnEntryTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (!OtherActor) return;

    ACharacter* OverlappingCharacter = Cast<ACharacter>(OtherActor);
    if (OverlappingCharacter)
    {
        EnteredPlayers.Add(OverlappingCharacter);

        FString Msg = FString::Printf(TEXT("Waiting: %d / 3"), EnteredPlayers.Num());
        if (HasAuthority()) GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::White, Msg);

        if (EnteredPlayers.Num() == 3)
        {
            if (EntryDoor)
                EntryDoor->Close();  // 문 닫고

            if (Manager)
                Manager->StartNextRoom();  // 다음 방 시작
        }
    }
}
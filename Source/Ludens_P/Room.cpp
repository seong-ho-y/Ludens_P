// Fill out your copyright notice in the Description page of Project Settings.


#include "Room.h"
#include "Door.h"
#include "RoomManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/Character.h"    // 플레이어 판별용
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

    if (GEngine)
    {
        FString Msg = FString::Printf(TEXT("Room %d Start"), RoomIndex);
        GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Yellow, Msg);
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
        GEngine->AddOnScreenDebugMessage(3, 5.f, FColor::Cyan, Msg);
    }

    if (Manager)
    {
        Manager->NotifyRoomCleared(RoomIndex);
    }

    // 모든 플레이어에게 보상 UI 띄우기
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = Cast<APlayerController>(*It))
        {
            ACharacter* PlayerChar = Cast<ACharacter>(PC->GetPawn());
            if (PlayerChar)
            {
                URewardSystemComponent* RewardComp = PlayerChar->FindComponentByClass<URewardSystemComponent>();
                if (RewardComp)
                {
                    RewardComp->Server_ShowRewardOptions();
                }
            }
        }
    }
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

    if (HasAuthority())
    {
        if (GEngine)
        {
            FString Msg = FString::Printf(TEXT("[Room] Waiting: %d / %d"), EnteredPlayers.Num(), RequiredPlayers);
            GEngine->AddOnScreenDebugMessage(4, 4.f, FColor::White, Msg);
        }

        if (EnteredPlayers.Num() == RequiredPlayers)
        {
            if (EntryDoor) EntryDoor->Close();      // 문 닫고
            if (Manager) Manager->StartNextRoom();  // 다음 방 시작
        }
    }
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Elevator.h"
#include "Door.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

// Sets default values
AElevator::AElevator()
{

}

// Called when the game starts or when spawned
void AElevator::BeginPlay()
{
    Super::BeginPlay();

    if (EntryDoor) EntryDoor->Close();
    if (ExitDoor) ExitDoor->Close();
}

void AElevator::OnEntryTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
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
            GEngine->AddOnScreenDebugMessage(-1, 4.f, FColor::White,
                FString::Printf(TEXT("[Elevator] Waiting: %d / 3"), EnteredPlayers.Num()));
        }

        if (EnteredPlayers.Num() == 3)
        {
            if (bIsStartElevator)
            {
                // RoomManager가 첫 방 EntryDoor를 열도록 신호
                OnAllPlayersReady.Broadcast();
            }
            else if (bIsEndElevator)
            {
                // 3초 후 다음 스테이지로
                FTimerHandle Handle;
                GetWorld()->GetTimerManager().SetTimer(
                    Handle, this, &AElevator::TravelToNextStage, 3.f, false);
            }
        }
    }
}

void AElevator::TravelToNextStage()
{
    // 멀티플레이 안전장치 : 서버에서만
    if (!HasAuthority())
    {
        UE_LOG(LogTemp, Warning, TEXT("Travel requested on client - ignored."));
        return;
    }

    if (!NextStage.IsValid() && NextStage.ToSoftObjectPath().IsNull())
    {
        UE_LOG(LogTemp, Error, TEXT("NextStage is not set!"));
        return;
    }

    // 소프트경로 -> 텍스트 URL (예: "/Game/Maps/Stage2")
    const FString MapPath = NextStage.ToSoftObjectPath().ToString();

    // 중복 호출 방지(타이머 2번 등 대비)
    static bool bTraveling = false;
    if (bTraveling) return;
    bTraveling = true;

    // 리슨 서버라면 "?listen" 유지가 필요할 수 있음
    const bool bIsListenServer = GetNetMode() != NM_DedicatedServer;
    const FString Url = bIsListenServer ? (MapPath + TEXT("?listen")) : MapPath;

    UE_LOG(LogTemp, Log, TEXT("ServerTravel to: %s"), *Url);
    GetWorld()->ServerTravel(Url, /*bAbsolute*/ false);

    // 주의: ServerTravel 이후 현재 월드는 곧 파괴됩니다. 여기서 추가 로직 실행 금지!
}
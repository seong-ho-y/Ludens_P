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

    // 소프트 경로 -> 패키지 경로("/Game/Maps/Stage2") 추출
    const FString ObjPath = NextStage.ToSoftObjectPath().ToString(); // "/Game/Maps/Stage2.Stage2"
    FString PackagePath, ObjectName;
    if (!ObjPath.Split(TEXT("."), &PackagePath, &ObjectName))
    {
        UE_LOG(LogTemp, Error, TEXT("AElevator::TravelToNextStage: Invalid map path: %s"), *ObjPath);
        return;
    }

    // 리슨 서버라면 계속 리슨 유지
    const bool bIsListenServer = (GetNetMode() != NM_DedicatedServer);
    const FString URL = bIsListenServer ? (PackagePath + TEXT("?listen")) : PackagePath;

    UE_LOG(LogTemp, Log, TEXT("ServerTravel to: %s"), *URL);

    // 멀티플레이 이동은 ServerTravel이 정석
    // (참고) Seamless Travel은 GameMode에서 bUseSeamlessTravel=true로 설정
    GetWorld()->ServerTravel(URL, /*bAbsolute*/ false);

    // 주의: 여기서부터 현재 월드는 곧 파괴되므로 추가 로직 금지
}

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

    // �θ𿡼� �°��� EnteredPlayers ���
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
                // RoomManager�� ù �� EntryDoor�� ������ ��ȣ
                OnAllPlayersReady.Broadcast();
            }
            else if (bIsEndElevator)
            {
                // 3�� �� ���� ����������
                FTimerHandle Handle;
                GetWorld()->GetTimerManager().SetTimer(
                    Handle, this, &AElevator::TravelToNextStage, 3.f, false);
            }
        }
    }
}

void AElevator::TravelToNextStage()
{
    // ��Ƽ�÷��� ������ġ : ����������
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

    // ����Ʈ ��� -> ��Ű�� ���("/Game/Maps/Stage2") ����
    const FString ObjPath = NextStage.ToSoftObjectPath().ToString(); // "/Game/Maps/Stage2.Stage2"
    FString PackagePath, ObjectName;
    if (!ObjPath.Split(TEXT("."), &PackagePath, &ObjectName))
    {
        UE_LOG(LogTemp, Error, TEXT("AElevator::TravelToNextStage: Invalid map path: %s"), *ObjPath);
        return;
    }

    // ���� ������� ��� ���� ����
    const bool bIsListenServer = (GetNetMode() != NM_DedicatedServer);
    const FString URL = bIsListenServer ? (PackagePath + TEXT("?listen")) : PackagePath;

    UE_LOG(LogTemp, Log, TEXT("ServerTravel to: %s"), *URL);

    // ��Ƽ�÷��� �̵��� ServerTravel�� ����
    // (����) Seamless Travel�� GameMode���� bUseSeamlessTravel=true�� ����
    GetWorld()->ServerTravel(URL, /*bAbsolute*/ false);

    // ����: ���⼭���� ���� ����� �� �ı��ǹǷ� �߰� ���� ����
}

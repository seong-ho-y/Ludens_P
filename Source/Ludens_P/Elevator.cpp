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

    if (ACharacter* Ch = Cast<ACharacter>(OtherActor))
    {
        EnteredPlayers.Add(Ch);

        if (HasAuthority())
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(4, 4.f, FColor::White,
                    FString::Printf(TEXT("[Elevator] Waiting: %d / %d"), EnteredPlayers.Num(), RequiredPlayers));
            }

            if (EnteredPlayers.Num() >= 2)
            {
                // Start/End ������ RoomManager�� �Ѵ�
                OnAllPlayersReady.Broadcast();
            }
        }
    }
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Room.h"
#include "Elevator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAllPlayersReady);

UCLASS()
class LUDENS_P_API AElevator : public ARoom
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElevator();

    // 3인 집결시 브로드캐스트 → RoomManager가 받음
    UPROPERTY(BlueprintAssignable, Category = "Elevator")
    FOnAllPlayersReady OnAllPlayersReady;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    // 플레이어가 다 모였을 때 호출
    virtual void OnEntryTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult) override;
};

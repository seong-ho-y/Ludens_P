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

    // 3�� ����� ��ε�ĳ��Ʈ �� RoomManager�� ����
    UPROPERTY(BlueprintAssignable, Category = "Elevator")
    FOnAllPlayersReady OnAllPlayersReady;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    // �÷��̾ �� ���� �� ȣ��
    virtual void OnEntryTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult) override;
};

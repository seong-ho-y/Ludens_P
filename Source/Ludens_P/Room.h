// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Room.generated.h"

class ARoomManager;
class ADoor;

UCLASS()
class LUDENS_P_API ARoom : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARoom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    // �� ���� (5�� �� �ڵ� Ŭ����)
    void StartRoom();

    // RoomManager���� ȣ��
    void SetManager(ARoomManager* InManager);
    void SetRoomIndex(int32 Index);

    // ���� ������ �� �ֵ��� ���� ����
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Room")
    ADoor* EntryDoor; // ���� Ŭ����Ǹ� ���� ��

protected:
    // �ڵ� Ŭ���� ó��
    void AutoClear();

private:
    UPROPERTY()
    ARoomManager* Manager;

    int32 RoomIndex = -1;
    bool bIsCleared = false;

    FTimerHandle AutoClearTimer;

public:
    // �� ���� üũ�� Ʈ����
    UPROPERTY(VisibleAnywhere, Category = "Room")
    UBoxComponent* EntryTrigger;

    // �Ա� �� �� (���� �� ���� �� ������ �� ��)

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Room")
    ADoor* ExitDoor;

    // Ʈ���� ������ ó��
    UFUNCTION()
    void OnEntryTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

private:
    TSet<AActor*> EnteredPlayers;  // �ߺ� ������ ���� ����

public:
    // � ������ ���� �������� �����ϴ� Ŭ���� ����
    UPROPERTY(EditAnywhere, Category = "Room")
    TSubclassOf<ADoor> DoorClass;
};
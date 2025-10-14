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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
    int32 RequiredPlayers = 3;

    // ���� ������ �� �ֵ��� ���� ����
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Door")
    ADoor* EntryDoor;   // �Ա� �� �� (���� �� ���� �� ������ �� ��)

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Door")
    ADoor* ExitDoor;    // ���� Ŭ����Ǹ� ���� ��

    // � ������ ���� �������� �����ϴ� Ŭ���� ����
    UPROPERTY(EditAnywhere, Category = "Door")
    TSubclassOf<ADoor> DoorClass;

    // �� ���� ��ġ
    UPROPERTY(EditAnywhere, Category = "Door|Layout")
    FVector2D EntryDoorOffset = FVector2D(-1000.f, 0.f);

    UPROPERTY(EditAnywhere, Category = "Door|Layout")
    FVector2D ExitDoorOffset = FVector2D(1000.f, 0.f);

    // �� ���� üũ�� Ʈ����
    UPROPERTY(VisibleAnywhere, Category = "Room")
    UBoxComponent* EntryTrigger;

    // �� ũ�� (X�� ����)
    UPROPERTY(EditAnywhere, Category = "Room|Layout")
    float RoomSize = 2000.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    TSet<AActor*> EnteredPlayers;  // �ߺ� ������ ���� ����

    // �ڵ� Ŭ���� ó��
    void AutoClear();

public:
    // �� ���� (5�� �� �ڵ� Ŭ����)
    void StartRoom();

    // RoomManager���� ȣ��
    void SetManager(ARoomManager* InManager);

    void SetRoomIndex(int32 Index);

    // Ʈ���� ������ ó��
    UFUNCTION()
    virtual void OnEntryTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
        const FHitResult& SweepResult);

    float GetHalfWidthX() const { return RoomSize * 0.5f; }
    FVector GetEntryDoorWorldPos() const { return GetActorLocation() + FVector(EntryDoorOffset.X, EntryDoorOffset.Y, 0.f); }
    FVector GetExitDoorWorldPos()  const { return GetActorLocation() + FVector(ExitDoorOffset.X, ExitDoorOffset.Y, 0.f); }

private:
    UPROPERTY()
    ARoomManager* Manager;

    int32 RoomIndex = -1;
    bool bIsCleared = false;

    FTimerHandle AutoClearTimer;
};
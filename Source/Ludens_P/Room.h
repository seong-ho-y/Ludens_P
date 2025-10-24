// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Room.generated.h"

class AEnemySpawnPoint;
class ARoomManager;
class ADoor;

UCLASS()
class LUDENS_P_API ARoom : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARoom();
    // Room.h
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy")
    TArray<AEnemySpawnPoint*> RoomSpawnPoints;


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
	virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    TSet<AActor*> EnteredPlayers;  // �ߺ� ������ ���� ����

    // void AutoClear();

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

protected:
    UPROPERTY() ARoomManager* Manager;

private:
    int32 RoomIndex = -1;
    bool bIsCleared = false;

    // ���� Ŭ��� ������ (BP���� ���� ����)
    UPROPERTY(EditDefaultsOnly, Category = "Room|Clear")
    float ClearDelaySeconds = 1.5f;   // 0.8~1.5s ��õ

    // �ߺ� ���� ����/��ҿ�
    bool bClearPending = false;
    FTimerHandle ClearDelayTimer;

    // ��� óġ �̺�Ʈ �ݹ�
    UFUNCTION() void HandleAllEnemiesKilled();

    void ScheduleClearWithDelay();   // ���� Ŭ���� ����
    void CancelPendingClear();       // �ʿ� �� ���� ���
    void DoClear();                  // ���� Ŭ���� ó��

public:
    UFUNCTION(BlueprintCallable, Category = "Room|Trigger")
    void DisableEntryTrigger(bool bDestroyComponent = true);

};
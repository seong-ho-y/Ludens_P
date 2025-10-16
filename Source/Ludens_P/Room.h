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

    // 문과 연결할 수 있도록 변수 노출
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Door")
    ADoor* EntryDoor;   // 입구 쪽 문 (다음 방 입장 후 닫히게 될 문)

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Door")
    ADoor* ExitDoor;    // 방이 클리어되면 열릴 문

    // 어떤 종류의 문을 생성할지 결정하는 클래스 변수
    UPROPERTY(EditAnywhere, Category = "Door")
    TSubclassOf<ADoor> DoorClass;

    // 문 생성 위치
    UPROPERTY(EditAnywhere, Category = "Door|Layout")
    FVector2D EntryDoorOffset = FVector2D(-1000.f, 0.f);

    UPROPERTY(EditAnywhere, Category = "Door|Layout")
    FVector2D ExitDoorOffset = FVector2D(1000.f, 0.f);

    // 방 입장 체크용 트리거
    UPROPERTY(VisibleAnywhere, Category = "Room")
    UBoxComponent* EntryTrigger;

    // 방 크기 (X축 기준)
    UPROPERTY(EditAnywhere, Category = "Room|Layout")
    float RoomSize = 2000.f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    TSet<AActor*> EnteredPlayers;  // 중복 방지를 위한 집합

    // 자동 클리어 처리
    void AutoClear();

public:
    // 방 시작 (5초 후 자동 클리어)
    void StartRoom();

    // RoomManager에서 호출
    void SetManager(ARoomManager* InManager);

    void SetRoomIndex(int32 Index);

    // 트리거 오버랩 처리
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
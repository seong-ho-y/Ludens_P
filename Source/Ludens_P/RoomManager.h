// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraShakeBase.h"
#include "RoomManager.generated.h"

class ARoom;
class AElevator;

UCLASS()
class LUDENS_P_API ARoomManager : public AActor
{
	GENERATED_BODY()

    // 생성된 방 인스턴스 저장
    UPROPERTY() TArray<ARoom*> SpawnedRooms;

protected:
    // 문 닫은 뒤 여행까지 대기 시간(블루프린트에서 조절 가능)
    UPROPERTY(EditAnywhere, Category = "Room|Travel")
    float DelayBeforeTravel = 6.0f;

    // ==================  추가: 카메라 쉐이크 세팅 ==================
    // 시작 시(스타트 엘리베이터 도착 느낌) 한 번 “덜컥”
    UPROPERTY(EditDefaultsOnly, Category = "FX|Camera")
    TSubclassOf<UCameraShakeBase> StartArrivalShake;

    // 엔드 엘리베이터 출발 느낌(조금 더 길고 저주파)
    UPROPERTY(EditDefaultsOnly, Category = "FX|Camera")
    TSubclassOf<UCameraShakeBase> DepartShake;

    // 강도 스케일(필요 시)
    UPROPERTY(EditDefaultsOnly, Category = "FX|Camera", meta = (ClampMin = "0.1", UIMin = "0.1"))
    float StartArrivalShakeScale = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "FX|Camera", meta = (ClampMin = "0.1", UIMin = "0.1"))
    float DepartShakeScale = 1.0f;

    UPROPERTY(EditAnywhere, Category = "FX|Camera")
    float StartArrivalInputLockSeconds = 0.6f; // 흔들림 길이에 맞춰 조정

public:	
	// Sets default values for this actor's properties
	ARoomManager();

    // 총 몇 개의 방을 만들 것인지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room")
    int32 NumRoomsToSpawn = 5;

    // 생성 가능한 방 종류들 (BP_Room1, BP_Room2 등)
    UPROPERTY(EditAnywhere, Category = "Room")
    TArray<TSubclassOf<ARoom>> RoomTypes;

    // 레벨에 배치한 엘리베이터 참조(인스턴스 지정)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room|Elevator")
    TSubclassOf<AElevator> Start;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room|Elevator")
    TSubclassOf<AElevator> End;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room|Layout")
    float GapBetweenRooms = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Room|Travel")
    TSoftObjectPtr<UWorld> NextStage;

    UPROPERTY() AElevator* StartElevator;
    UPROPERTY() AElevator* EndElevator;

private:
    void GenerateRooms(); // 방을 생성하는 함수

    void LayoutChain();

    void TravelToNextStage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    // 전 플레이어에게 흔들림을 재생하는 헬퍼
    void StartShake_AllPlayers(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale = 1.f) const;

    void LockAllPlayerInputs(bool bLock);

public:
    UFUNCTION() void HandleStartElevatorReady();
    UFUNCTION() void HandleEndElevatorReady();

    // 첫 방의 EntryDoor를 열어주는 함수
    UFUNCTION() void OpenFirstRoomEntryDoor();

    // 다음 방을 시작시키는 공개 함수 (ARoom이 호출)
    UFUNCTION() void StartNextRoom();

    // 현재 방을 클리어 처리함 (ARoom에서 알림 받음)
    UFUNCTION() void NotifyRoomCleared(int32 RoomIndex);

private:
    // 현재 플레이어가 위치한 방의 인덱스
    int32 CurrentRoomIndex = 0;

    // 중복 호출 방지용 타이머
    FTimerHandle TravelTimer;
};

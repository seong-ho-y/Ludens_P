// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomManager.generated.h"

class ARoom; // Room 클래스 포워드 선언

UCLASS()
class LUDENS_P_API ARoomManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoomManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    // 다음 방을 시작시키는 공개 함수 (ARoom이 호출)
    UFUNCTION()
    void StartNextRoom();

    // 현재 방을 클리어 처리함 (ARoom에서 알림 받음)
    UFUNCTION()
    void NotifyRoomCleared(int32 RoomIndex);

    // 총 몇 개의 방을 만들 것인지
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Generation")
    int32 NumRoomsToSpawn = 5;

    // 생성 가능한 방 종류들 (BP_Room1, BP_Room2 등)
    UPROPERTY(EditAnywhere, Category = "Room Generation")
    TArray<TSubclassOf<ARoom>> RoomTypes;

    // 방 간 거리 (X축 기준)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Generation")
    float RoomSpacing = 2000.f;

private:
    // 생성된 방 인스턴스 저장
    UPROPERTY()
    TArray<ARoom*> SpawnedRooms;

    // 현재 플레이어가 위치한 방의 인덱스
    int32 CurrentRoomIndex = 0;

    // 방을 생성하는 함수
    void GenerateRooms();
};

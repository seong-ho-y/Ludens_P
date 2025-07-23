// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomManager.generated.h"

class ARoom; // Room Ŭ���� ������ ����

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
    // ���� ���� ���۽�Ű�� ���� �Լ� (ARoom�� ȣ��)
    UFUNCTION()
    void StartNextRoom();

    // ���� ���� Ŭ���� ó���� (ARoom���� �˸� ����)
    UFUNCTION()
    void NotifyRoomCleared(int32 RoomIndex);

    // �� �� ���� ���� ���� ������
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Generation")
    int32 NumRoomsToSpawn = 5;

    // ���� ������ �� ������ (BP_Room1, BP_Room2 ��)
    UPROPERTY(EditAnywhere, Category = "Room Generation")
    TArray<TSubclassOf<ARoom>> RoomTypes;

    // �� �� �Ÿ� (X�� ����)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room Generation")
    float RoomSpacing = 2000.f;

private:
    // ������ �� �ν��Ͻ� ����
    UPROPERTY()
    TArray<ARoom*> SpawnedRooms;

    // ���� �÷��̾ ��ġ�� ���� �ε���
    int32 CurrentRoomIndex = 0;

    // ���� �����ϴ� �Լ�
    void GenerateRooms();
};

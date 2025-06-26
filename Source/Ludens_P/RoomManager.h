// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomManager.generated.h"

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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARoom> RoomClass;

public:
	UFUNCTION()
	void NotifyRoomCleared(class ARoom* ClearedRoom);

private:
	TArray<ARoom*> RoomInstances;
	int32 RoomCount = 0;
};

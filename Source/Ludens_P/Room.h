// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Room.generated.h"

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

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room", meta = (AllowPrivateAccess = "true"))
	class ADoor* ExitDoor;

	FTimerHandle ClearTimerHandle;

	bool bIsCleared = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Room", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ARoom> RoomClass;

	AActor* TriggerVolume; // BoxTrigger µî

	TSet<APlayerController*> OverlappedPlayers;
	int32 RequiredPlayersToProceed = 3;

	void SimulateRoomClear();
	void SpawnNextRoom();
	void RemoveCurrentRoom();

	UFUNCTION()
	void OnTriggerEnter(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnTriggerExit(AActor* OverlappedActor, AActor* OtherActor);
};
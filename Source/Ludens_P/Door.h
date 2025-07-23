// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

UCLASS()
class LUDENS_P_API ADoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void Open();

	UFUNCTION(BlueprintCallable)
	void Close();

private:
	// 문 외형
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DoorMesh;

	// 현재 열린 상태인지 여부
	bool bIsOpen = false;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOpen();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastClose();
};

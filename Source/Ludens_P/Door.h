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

<<<<<<< Updated upstream
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
=======
    //문 열림 상태를 복제 + 변경 시 함수 호출
    UPROPERTY(ReplicatedUsing = OnRep_DoorStateChanged)
    bool bIsOpen;

    UFUNCTION()
    void OnRep_DoorStateChanged();  // 클라이언트에서만 호출됨

    // 문 시각 + 충돌 처리
    void ApplyDoorState();

public:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* DoorMesh;

    UFUNCTION(BlueprintCallable)
    void Open();  // 문 열기

    UFUNCTION(BlueprintCallable)
    void Close(); // 문 닫기

    // 복제 설정 함수
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
>>>>>>> Stashed changes
};

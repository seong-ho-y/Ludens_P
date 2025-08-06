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
	// �� ����
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DoorMesh;

	// ���� ���� �������� ����
	bool bIsOpen = false;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastOpen();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastClose();
=======
    //�� ���� ���¸� ���� + ���� �� �Լ� ȣ��
    UPROPERTY(ReplicatedUsing = OnRep_DoorStateChanged)
    bool bIsOpen;

    UFUNCTION()
    void OnRep_DoorStateChanged();  // Ŭ���̾�Ʈ������ ȣ���

    // �� �ð� + �浹 ó��
    void ApplyDoorState();

public:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* DoorMesh;

    UFUNCTION(BlueprintCallable)
    void Open();  // �� ����

    UFUNCTION(BlueprintCallable)
    void Close(); // �� �ݱ�

    // ���� ���� �Լ�
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
>>>>>>> Stashed changes
};

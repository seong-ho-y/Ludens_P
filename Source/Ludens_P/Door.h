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

    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* DoorMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    //문 열림 상태를 복제 + 변경 시 함수 호출
    UPROPERTY(ReplicatedUsing = OnRep_DoorStateChanged)
    bool bIsOpen;

    UFUNCTION()
    void OnRep_DoorStateChanged();  // 클라이언트에서만 호출됨

    // 문 시각/충돌 처리 → 애니메이션 기반으로 재구성
    void StartOpenAnim();
    void StartCloseAnim();

    // Tick으로 애니메이션 구동
    virtual void Tick(float DeltaSeconds) override;

    // ---- 애니메이션 파라미터 ----
    UPROPERTY(EditAnywhere, Category = "Door|Anim")
    float OpenDuration = 0.6f;

    UPROPERTY(EditAnywhere, Category = "Door|Anim")
    float SlideDistance = 220.f; // 아래로 내려갈 거리(+Z는 위, 우리는 -Z로 내림)

    bool  bAnimating = false;
    bool  bAnimOpening = false;
    float AnimElapsed = 0.f;

    FVector ClosedRelLocation; // 시작(닫힘) 위치
    FVector OpenRelLocation;   // 완전 개방 위치(닫힘 기준 -Z

public:
    UFUNCTION(BlueprintCallable)
    void Open();    // 문 열기

    UFUNCTION(BlueprintCallable)
    void Close();   // 문 닫기

    // 복제 설정 함수
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION(BlueprintPure, Category = "Door")
    float GetAnimDuration() const { return OpenDuration; }
};

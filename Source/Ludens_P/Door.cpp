// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Net/UnrealNetwork.h"

ADoor::ADoor()
{
    bReplicates = true;
    SetReplicateMovement(true); // 문이 움직이는 경우

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
    DoorMesh->SetupAttachment(RootComponent);
    DoorMesh->SetCollisionProfileName(TEXT("BlockAll"));    // 초기 충돌 설정

    bIsOpen = false;

    // Tick으로 애니메이션 갱신
    PrimaryActorTick.bCanEverTick = true;
}

void ADoor::BeginPlay()
{
    Super::BeginPlay();

    // 기준 위치 기록
    ClosedRelLocation = DoorMesh ? DoorMesh->GetRelativeLocation() : FVector::ZeroVector;
    OpenRelLocation = ClosedRelLocation + FVector(0, 0, -SlideDistance);

    // 초기 상태 반영(애니메이션 없이 위치만 맞춤)
    if (bIsOpen)
    {
        if (DoorMesh) DoorMesh->SetRelativeLocation(OpenRelLocation);
        SetActorEnableCollision(false);
        DoorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        DoorMesh->SetVisibility(false);
        DoorMesh->SetHiddenInGame(true);
    }
    else
    {
        if (DoorMesh) DoorMesh->SetRelativeLocation(ClosedRelLocation);
        SetActorEnableCollision(true);
        DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        DoorMesh->SetVisibility(true);
        DoorMesh->SetHiddenInGame(false);
    }
}

void ADoor::Open()
{
    if (HasAuthority() && !bIsOpen)
    {
        bIsOpen = true;      // OnRep에서 클라 애니메이션 시작
        OnRep_DoorStateChanged(); // 서버에서도 즉시 애니 시작
    }
}

void ADoor::Close()
{
    if (HasAuthority() && bIsOpen)
    {
        bIsOpen = false;
        OnRep_DoorStateChanged(); // 서버에서도 즉시 애니 시작
    }
}

void ADoor::OnRep_DoorStateChanged()
{
    if (bIsOpen) StartOpenAnim();
    else         StartCloseAnim();
}

void ADoor::StartOpenAnim()
{
    if (!DoorMesh) return;

    // 애니 초기화
    bAnimating = true;
    bAnimOpening = true;
    AnimElapsed = 0.f;

    // 열기 시작 즉시 충돌 Off(플레이어 통과 가능)
    SetActorEnableCollision(false);
    DoorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 보이도록 두고 내려가면서 마지막에 숨김
    DoorMesh->SetVisibility(true);
    DoorMesh->SetHiddenInGame(false);
}

void ADoor::StartCloseAnim()
{
    if (!DoorMesh) return;

    bAnimating = true;
    bAnimOpening = false;
    AnimElapsed = 0.f;

    // 닫기 시작 시 보이게
    DoorMesh->SetVisibility(true);
    DoorMesh->SetHiddenInGame(false);
}

void ADoor::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (!bAnimating || !DoorMesh) return;

    AnimElapsed += DeltaSeconds;
    const float T = FMath::Clamp(AnimElapsed / FMath::Max(0.01f, OpenDuration), 0.f, 1.f);

    // 부드러운 이징(원하면 다른 이징으로 교체)
    const float Alpha = FMath::InterpEaseInOut(0.f, 1.f, T, 2.0f);

    if (bAnimOpening)
    {
        const FVector NewLoc = FMath::Lerp(ClosedRelLocation, OpenRelLocation, Alpha);
        DoorMesh->SetRelativeLocation(NewLoc);

        if (T >= 1.f)
        {
            // 완전 열림: 보조로 숨김(선택사항)
            DoorMesh->SetHiddenInGame(true);
            DoorMesh->SetVisibility(false);
            bAnimating = false;
        }
    }
    else
    {
        const FVector NewLoc = FMath::Lerp(OpenRelLocation, ClosedRelLocation, Alpha);
        DoorMesh->SetRelativeLocation(NewLoc);

        if (T >= 1.f)
        {
            // 완전 닫힘: 충돌 복구
            SetActorEnableCollision(true);
            DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            bAnimating = false;
        }
    }
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ADoor, bIsOpen);
}

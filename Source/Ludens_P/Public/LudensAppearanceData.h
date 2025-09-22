// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "LobbyTypes.h" // ELobbyColor
#include "Engine/DataAsset.h"
#include "LudensAppearanceData.generated.h"

// 전방 선언(컴파일 속도/의존성 최소화)
class USkeletalMesh;
class UMaterialInterface;

/** 외형 한 종(A0~A3 등)에 대한 머티리얼 세트(Default/Red/Green/Blue) */
USTRUCT(BlueprintType)
struct FAppearanceMatSet
{
    GENERATED_BODY()
    // Default 슬롯은 더 이상 쓰지 않지만, 레거시 호환을 위해 남겨둠.
    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    UMaterialInterface* Default = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    UMaterialInterface* Red = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    UMaterialInterface* Green = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    UMaterialInterface* Blue = nullptr;
};

/** 로비/인게임이 함께 참조하는 외형/색상 DB (Single Source of Truth) */
UCLASS(BlueprintType)
class LUDENS_P_API ULudensAppearanceData : public UDataAsset
{
    GENERATED_BODY()

public:
    // 외형별 3P 메쉬
    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    TArray<USkeletalMesh*> AppearanceMeshes;

    // [기존] 외형별 "캐릭터 색" 세트 (Default/Red/Green/Blue)
    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    TArray<FAppearanceMatSet> AppearanceMaterials;

    // [신규] 외형별 "무기 색" 세트 (Default/Red/Green/Blue)
    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    TArray<FAppearanceMatSet> WeaponMaterials;

    // [신규] 외형별 슬롯 인덱스(없으면 -1)
    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics|Slots")
    TArray<int32> CharacterSlotIndex;

    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics|Slots")
    TArray<int32> WeaponSlotIndex;
    // (선택) 슬롯 이름까지 쓸 계획이면 여기에 FName 배열도 추가해 이름 우선/인덱스 폴백 가능    

public:
    /** 메쉬 교체 + [캐릭터/무기] 슬롯에 색상별 머티리얼 적용 (프리뷰/인게임 공용) */
    UFUNCTION(BlueprintCallable, Category = "Cosmetics")
    void ApplyTo(class USkeletalMeshComponent* MeshComp, int32 AppearanceIdx, ELobbyColor Color) const;
};
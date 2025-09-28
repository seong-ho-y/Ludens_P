// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "LobbyTypes.h" // ELobbyColor
#include "Engine/DataAsset.h"
#include "LudensAppearanceData.generated.h"

// ���� ����(������ �ӵ�/������ �ּ�ȭ)
class USkeletalMesh;
class UMaterialInterface;

/** ���� �� ��(A0~A3 ��)�� ���� ��Ƽ���� ��Ʈ(Default/Red/Green/Blue) */
USTRUCT(BlueprintType)
struct FAppearanceMatSet
{
    GENERATED_BODY()
    // Default ������ �� �̻� ���� ������, ���Ž� ȣȯ�� ���� ���ܵ�.
    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    UMaterialInterface* Default = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    UMaterialInterface* Red = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    UMaterialInterface* Green = nullptr;

    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    UMaterialInterface* Blue = nullptr;
};

/** �κ�/�ΰ����� �Բ� �����ϴ� ����/���� DB (Single Source of Truth) */
UCLASS(BlueprintType)
class LUDENS_P_API ULudensAppearanceData : public UDataAsset
{
    GENERATED_BODY()

public:
    // ������ 3P �޽�
    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    TArray<USkeletalMesh*> AppearanceMeshes;

    // [����] ������ "ĳ���� ��" ��Ʈ (Default/Red/Green/Blue)
    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    TArray<FAppearanceMatSet> AppearanceMaterials;

    // [�ű�] ������ "���� ��" ��Ʈ (Default/Red/Green/Blue)
    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics")
    TArray<FAppearanceMatSet> WeaponMaterials;

    // [�ű�] ������ ���� �ε���(������ -1)
    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics|Slots")
    TArray<int32> CharacterSlotIndex;

    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics|Slots")
    TArray<int32> WeaponSlotIndex;
    // (����) ���� �̸����� �� ��ȹ�̸� ���⿡ FName �迭�� �߰��� �̸� �켱/�ε��� ���� ����    

public:
    /** �޽� ��ü + [ĳ����/����] ���Կ� ���� ��Ƽ���� ���� (������/�ΰ��� ����) */
    UFUNCTION(BlueprintCallable, Category = "Cosmetics")
    void ApplyTo(class USkeletalMeshComponent* MeshComp, int32 AppearanceIdx, ELobbyColor Color) const;
};
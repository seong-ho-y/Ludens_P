#include "LudensAppearanceData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInterface.h"

static UMaterialInterface* PickByColor_DefaultIsRed(const FAppearanceMatSet& Set, ELobbyColor C)
{
    // Default(=색 미선택)도 Red 취급
    switch (C)
    {
    case ELobbyColor::Red:
    case ELobbyColor::None:   return Set.Red ? Set.Red : (Set.Green ? Set.Green : (Set.Blue ? Set.Blue : Set.Default));
    case ELobbyColor::Green:  return Set.Green ? Set.Green : (Set.Red ? Set.Red : (Set.Blue ? Set.Blue : Set.Default));
    case ELobbyColor::Blue:   return Set.Blue ? Set.Blue : (Set.Red ? Set.Red : (Set.Green ? Set.Green : Set.Default));
    default:                  return Set.Red ? Set.Red : (Set.Green ? Set.Green : (Set.Blue ? Set.Blue : Set.Default));
    }
}

static ELobbyColor ToLobbyColor(EEnemyColor C)
{
    switch (C)
    {
    case EEnemyColor::Red:   return ELobbyColor::Red;
    case EEnemyColor::Green: return ELobbyColor::Green;
    case EEnemyColor::Blue:  return ELobbyColor::Blue;
    default:                 return ELobbyColor::Red; // 폴백
    }
}

void ULudensAppearanceData::ApplyTo(USkeletalMeshComponent* MeshComp, int32 Ap, ELobbyColor Color) const
{
    if (!MeshComp) return;

    // 1) 메쉬 교체 (있으면)
    if (AppearanceMeshes.IsValidIndex(Ap))
    {
        if (USkeletalMesh* NewMesh = AppearanceMeshes[Ap])
        {
            if (MeshComp->GetSkeletalMeshAsset() != NewMesh)
            {
                MeshComp->SetSkeletalMesh(NewMesh);
            }
        }
    }

    // 2) 캐릭터 슬롯에 캐릭터 색 적용
    if (CharacterSlotIndex.IsValidIndex(Ap) && CharacterSlotIndex[Ap] >= 0
        && AppearanceMaterials.IsValidIndex(Ap))
    {
        const int32 Slot = CharacterSlotIndex[Ap];
        if (UMaterialInterface* M = PickByColor_DefaultIsRed(AppearanceMaterials[Ap], Color))
        {
            MeshComp->SetMaterial(Slot, M);
        }
    }

    // 3) 무기 슬롯에 무기 색 적용
    if (WeaponSlotIndex.IsValidIndex(Ap) && WeaponSlotIndex[Ap] >= 0
        && WeaponMaterials.IsValidIndex(Ap))
    {
        const int32 Slot = WeaponSlotIndex[Ap];
        if (UMaterialInterface* M = PickByColor_DefaultIsRed(WeaponMaterials[Ap], Color))
        {
            MeshComp->SetMaterial(Slot, M);
        }
    }

    // 나머지 슬롯(유리/화이트/그레이/블랙 등)은 건드리지 않음
}

void ULudensAppearanceData::ApplyToByEnemyColor(USkeletalMeshComponent* MeshComp, int32 AppearanceIdx, EEnemyColor EnemyColor) const
{
    // 내부적으로 기존 ELobbyColor 기반 구현을 재사용
    ApplyTo(MeshComp, AppearanceIdx, ToLobbyColor(EnemyColor));
}
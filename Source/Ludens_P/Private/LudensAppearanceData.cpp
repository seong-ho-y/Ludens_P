#include "LudensAppearanceData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInterface.h"

static UMaterialInterface* PickByColor_DefaultIsRed(const FAppearanceMatSet& Set, EEnemyColor C)
{
    // Default(=�� �̼���)�� Red ���
    switch (C)
    {
    case EEnemyColor::Red:
        return Set.Red
            ? Set.Red
            : (Set.Green ? Set.Green : (Set.Blue ? Set.Blue : Set.Default));

    case EEnemyColor::Green:
        return Set.Green
            ? Set.Green
            : (Set.Red ? Set.Red : (Set.Blue ? Set.Blue : Set.Default));

    case EEnemyColor::Blue:
        return Set.Blue
            ? Set.Blue
            : (Set.Red ? Set.Red : (Set.Green ? Set.Green : Set.Default));

    default:
        // ���� ����: Red �켱
        return Set.Red
            ? Set.Red
            : (Set.Green ? Set.Green : (Set.Blue ? Set.Blue : Set.Default));
    }
}

void ULudensAppearanceData::ApplyTo(USkeletalMeshComponent* MeshComp, int32 Ap, EEnemyColor Color) const
{
    if (!MeshComp) return;

    // 1) �޽� ��ü (������)
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

    // 2) ĳ���� ���Կ� ĳ���� �� ����
    if (CharacterSlotIndex.IsValidIndex(Ap) && CharacterSlotIndex[Ap] >= 0
        && AppearanceMaterials.IsValidIndex(Ap))
    {
        const int32 Slot = CharacterSlotIndex[Ap];
        if (UMaterialInterface* M = PickByColor_DefaultIsRed(AppearanceMaterials[Ap], Color))
        {
            MeshComp->SetMaterial(Slot, M);
        }
    }

    // 3) ���� ���Կ� ���� �� ����
    if (WeaponSlotIndex.IsValidIndex(Ap) && WeaponSlotIndex[Ap] >= 0
        && WeaponMaterials.IsValidIndex(Ap))
    {
        const int32 Slot = WeaponSlotIndex[Ap];
        if (UMaterialInterface* M = PickByColor_DefaultIsRed(WeaponMaterials[Ap], Color))
        {
            MeshComp->SetMaterial(Slot, M);
        }
    }

    // ������ ����(����/ȭ��Ʈ/�׷���/�� ��)�� �ǵ帮�� ����
}


void ULudensAppearanceData::ApplyToByEnemyColor(USkeletalMeshComponent* MeshComp, int32 AppearanceIdx, EEnemyColor EnemyColor) const
{
    // ���������� ���� ELobbyColor ��� ������ ����
    ApplyTo(MeshComp, AppearanceIdx, EnemyColor);
}
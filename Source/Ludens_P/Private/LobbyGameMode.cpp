// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "LobbyGameState.h" 
#include "Kismet/GameplayStatics.h"

#include "PlayerState_Real.h" // ���� �Ϸ�
#include "Ludens_P/EEnemyColor.h"
#include "LobbyTypes.h"

namespace
{
    static EEnemyColor ToEnemyColor(ELobbyColor C)
    {
        switch (C)
        {
        case ELobbyColor::Red:   return EEnemyColor::Red;
        case ELobbyColor::Green: return EEnemyColor::Green;
        case ELobbyColor::Blue:  return EEnemyColor::Blue;
        default:                 return EEnemyColor::Red; // ����
        }
    }
}

ALobbyGameMode::ALobbyGameMode()
{
    // S5���� ���ߴ� ��ȯ �����̹Ƿ� �̸� �ѵ�
    bUseSeamlessTravel = true;
    PlayerStateClass = APlayerState_Real::StaticClass();

    // �ʿ�� ���⼭ DefaultPawnClass / HUDClass / PlayerControllerClass ���� ����
    // ( BP���� PlayerControllerClass�� �����ϴ� ����� ����)
}



void ALobbyGameMode::StartGameIfAllReady()
{
    if (!HasAuthority()) return;

    // 1) �κ� ���� �˻�
    ALobbyGameState* GS = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
    if (!GS)
    {
        UE_LOG(LogTemp, Warning, TEXT("StartGameIfAllReady: No LobbyGameState"));
        return;
    }

    const int32 Total = GS->PlayerArray.Num();
    const int32 Ready = GS->ReadyCount;
    const bool  bAllReady = (Total > 0 && Ready == Total);

    if (!bAllReady)
    {
        UE_LOG(LogTemp, Warning, TEXT("StartGameIfAllReady: Not all ready (%d/%d)"), Ready, Total);
        return;
    }

    // 2) ������ �� ��ȿ��
    if (!(StageMap.IsValid() || StageMap.ToSoftObjectPath().IsValid()))
    {
        UE_LOG(LogTemp, Error, TEXT("StageMap is not set or invalid."));
        return;
    }

    // 3) �� ���⼭ '�� �� ����' �� Ȯ�� (ELobbyColor -> EEnemyColor)
    auto ToEnemyColor = [](ELobbyColor C)
        {
            switch (C)
            {
            case ELobbyColor::Red:   return EEnemyColor::Red;
            case ELobbyColor::Green: return EEnemyColor::Green;
            case ELobbyColor::Blue:  return EEnemyColor::Blue;
            default:                 return EEnemyColor::Red; // ����(������Ʈ ��Ģ�� �°�)
            }
        };

    for (APlayerState* PSBase : GS->PlayerArray)
    {
        if (auto* PS = Cast<APlayerState_Real>(PSBase))
        {
            // ���� 1ȸ Ŀ��
            PS->PlayerColor = ToEnemyColor(PS->SelectedColor);

            // (����) ���� ���ȭ�� ���ϸ� ����, �ƴϸ� �ּ�ó�� ����
            PS->ForceNetUpdate();

            UE_LOG(LogTemp, Display, TEXT("[PreTravel] %s Appear=%d Selected(ELobby)=%d -> PlayerColor(Enemy)=%d Skill=%d Ready=%d"),
                *GetNameSafe(PS), PS->AppearanceId, (int)PS->SelectedColor, (int)PS->PlayerColor, PS->SubskillId, (int)PS->bReady);
        }
    }

    // 4) �� ��ȯ (Seamless)
    const FString MapPath = StageMap.GetLongPackageName();
    const FString Url = MapPath + TEXT("?listen");
    UE_LOG(LogTemp, Display, TEXT("All ready -> ServerTravel to %s"), *Url);

    GetWorld()->ServerTravel(Url);
}



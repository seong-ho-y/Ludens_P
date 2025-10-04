// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "LobbyGameState.h"        
#include "PlayerState_Real.h" // ���� �Ϸ�



ALobbyGameMode::ALobbyGameMode()
{
    // S5���� ���ߴ� ��ȯ �����̹Ƿ� �̸� �ѵ�
    bUseSeamlessTravel = true;
    PlayerStateClass = APlayerState_Real::StaticClass();

    // �ʿ�� ���⼭ DefaultPawnClass / HUDClass / PlayerControllerClass ���� ����
    // (�̹� �ܰ迡�� BP���� PlayerControllerClass�� �����ϴ� ����� ����)
}

void ALobbyGameMode::StartGameIfAllReady()
{
    if (!HasAuthority()) return;

    ALobbyGameState* GS = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
    if (!GS) { UE_LOG(LogTemp, Warning, TEXT("StartGameIfAllReady: No LobbyGameState")); return; }

    const int32 Total = GS->PlayerArray.Num();
    const int32 Ready = GS->ReadyCount;
    const bool bAllReady = (Total > 0 && Ready == Total);
    if (!bAllReady)
    {
        UE_LOG(LogTemp, Warning, TEXT("StartGameIfAllReady: Not all ready (%d/%d)"), Ready, Total);
        return;
    }

    if (StageMap.IsValid() || StageMap.ToSoftObjectPath().IsValid())
    {
        const FString MapPath = StageMap.GetLongPackageName();
        const FString Url = MapPath + TEXT("?listen");

        // �� �κ�� �� �󼺻� ����(�� ���� ���� ���� ����)
        auto MapToEnemyColor = [](ELobbyColor C)
            {
                switch (C) {
                case ELobbyColor::Red:   return EEnemyColor::Red;
                case ELobbyColor::Green: return EEnemyColor::Green;
                case ELobbyColor::Blue:  return EEnemyColor::Blue;
                default:                 return EEnemyColor::Red; // ����
                }
            };

        // �� ServerTravel ����: ���� 1ȸ Ŀ�� + �α�
        for (APlayerState* Base : GameState->PlayerArray)
        {
            if (auto* PS = Cast<APlayerState_Real>(Base)) // ĳ���� ��� ��ü
            {
                PS->PlayerColor = MapToEnemyColor(PS->SelectedColor); //  ������ Ȯ��
                PS->ForceNetUpdate(); // (����) �����ϰ� �� �� �о���

                UE_LOG(LogTemp, Display, TEXT("[PreTravel] %s Appear=%d Sel=%d Prev=%d Skill=%d Ready=%d -> PlayerColor=%d"),
                    *GetNameSafe(PS),
                    PS->AppearanceId, (int)PS->SelectedColor, (int)PS->PreviewColor,
                    PS->SubskillId, PS->bReady, (int)PS->PlayerColor);
            }
        }

        UE_LOG(LogTemp, Display, TEXT("All ready -> ServerTravel to %s"), *Url);
        GetWorld()->ServerTravel(Url);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("StageMap is not set or invalid."));
    }
}


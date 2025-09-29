// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "LobbyGameState.h"        
#include "LobbyPlayerState.h"



ALobbyGameMode::ALobbyGameMode()
{
    // S5���� ���ߴ� ��ȯ �����̹Ƿ� �̸� �ѵ�
    bUseSeamlessTravel = true;
    PlayerStateClass = ALobbyPlayerState::StaticClass();

    // �ʿ�� ���⼭ DefaultPawnClass / HUDClass / PlayerControllerClass ���� ����
    // (�̹� �ܰ迡�� BP���� PlayerControllerClass�� �����ϴ� ����� ����)
}

void ALobbyGameMode::StartGameIfAllReady()
{
    if (!HasAuthority()) return;

    // �� GameMode ����� GetGameState()�� �ƴ϶�, UWorld ���ø� ������ ���
    ALobbyGameState* GS = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
    if (!GS)
    {
        UE_LOG(LogTemp, Warning, TEXT("StartGameIfAllReady: No LobbyGameState"));
        return;
    }

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
        // "/Game/Maps/TestMap"  �� ��(.) ���� '�� ��Ű�� �̸�'��
        const FString MapPath = StageMap.GetLongPackageName();

        const FString Url = MapPath + TEXT("?listen");
        UE_LOG(LogTemp, Display, TEXT("All ready -> ServerTravel to %s"), *Url);

        for (APlayerState* PS : GameState->PlayerArray)
        {
            if (auto* LPS = Cast<ALobbyPlayerState>(PS))
            {
                UE_LOG(LogTemp, Display, TEXT("[PreTravel] %s Appear=%d Sel=%d Prev=%d Skill=%d Ready=%d"),
                    *GetNameSafe(LPS),
                    LPS->AppearanceId, (int)LPS->SelectedColor, (int)LPS->PreviewColor,
                    LPS->SubskillId, LPS->bReady);
            }
        }

        GetWorld()->ServerTravel(Url);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("StageMap is not set or invalid."));
    }

}

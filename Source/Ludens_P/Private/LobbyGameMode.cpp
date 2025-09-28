// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "LobbyGameState.h"        
#include "LobbyPlayerState.h"



ALobbyGameMode::ALobbyGameMode()
{
    // S5에서 무중단 전환 예정이므로 미리 켜둠
    bUseSeamlessTravel = true;
    PlayerStateClass = ALobbyPlayerState::StaticClass();

    // 필요시 여기서 DefaultPawnClass / HUDClass / PlayerControllerClass 지정 가능
    // (이번 단계에선 BP에서 PlayerControllerClass를 지정하는 방식을 권장)
}

void ALobbyGameMode::StartGameIfAllReady()
{
    if (!HasAuthority()) return;

    // ★ GameMode 멤버의 GetGameState()가 아니라, UWorld 템플릿 버전을 사용
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
        // "/Game/Maps/TestMap"  ← 점(.) 없는 '긴 패키지 이름'만
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

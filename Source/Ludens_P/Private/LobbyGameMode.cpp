// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "LobbyGameState.h" 
#include "Kismet/GameplayStatics.h"

#include "PlayerState_Real.h" // 수정 완료
#include "Ludens_P/EEnemyColor.h"
#include "LobbyTypes.h"


ALobbyGameMode::ALobbyGameMode()
{
    // S5에서 무중단 전환 예정이므로 미리 켜둠
    bUseSeamlessTravel = true;
    PlayerStateClass = APlayerState_Real::StaticClass();

    // 필요시 여기서 DefaultPawnClass / HUDClass / PlayerControllerClass 지정 가능
    // ( BP에서 PlayerControllerClass를 지정하는 방식을 권장)
}


static FString ColorToStr(EEnemyColor C)
{
    if (const UEnum* E = StaticEnum<EEnemyColor>())
        return E->GetNameStringByValue((int64)C);
    return TEXT("Unknown");
}

void ALobbyGameMode::StartGameIfAllReady()
{
    if (!HasAuthority()) return;

    ALobbyGameState* GS = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
    if (!GS) { UE_LOG(LogTemp, Warning, TEXT("StartGameIfAllReady: No LobbyGameState")); return; }

    const int32 Total = GS->PlayerArray.Num();
    const int32 Ready = GS->ReadyCount;
    if (!(Total > 0 && Ready == Total))
    {
        UE_LOG(LogTemp, Warning, TEXT("StartGameIfAllReady: Not all ready (%d/%d)"), Ready, Total);
        return;
    }

    if (!(StageMap.IsValid() || StageMap.ToSoftObjectPath().IsValid()))
    {
        UE_LOG(LogTemp, Error, TEXT("StageMap is not set/invalid."));
        return;
    }

    // ? 여기서 '단 1회'만 커밋. 그 외 색 배정 로직(재배정/로테이션 등)은 전부 제거!
    for (APlayerState* PSBase : GS->PlayerArray)
    {
        if (APlayerState_Real* PSR = Cast<APlayerState_Real>(PSBase))
        {
            PSR->PlayerColor = PSR->SelectedColor;   // ?? 1회 커밋
            PSR->ForceNetUpdate();

            UE_LOG(LogTemp, Display, TEXT("[PreTravel] PS=%p Ap=%d Sel=%s -> PlayerColor=%s Name=%s"),
                PSR, PSR->AppearanceId, *ColorToStr(PSR->SelectedColor), *ColorToStr(PSR->PlayerColor), *PSR->GetPlayerName());

        }
    }

    // ? Seamless ServerTravel
    const FString MapPath = StageMap.GetLongPackageName();
    const FString URL = MapPath + TEXT("?listen");
    UE_LOG(LogTemp, Display, TEXT("All ready -> ServerTravel to %s"), *URL);
    GetWorld()->ServerTravel(URL);
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "LobbyGameState.h" 
#include "Kismet/GameplayStatics.h"

#include "PlayerState_Real.h" // 수정 완료
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
        default:                 return EEnemyColor::Red; // 폴백
        }
    }
}

ALobbyGameMode::ALobbyGameMode()
{
    // S5에서 무중단 전환 예정이므로 미리 켜둠
    bUseSeamlessTravel = true;
    PlayerStateClass = APlayerState_Real::StaticClass();

    // 필요시 여기서 DefaultPawnClass / HUDClass / PlayerControllerClass 지정 가능
    // ( BP에서 PlayerControllerClass를 지정하는 방식을 권장)
}



void ALobbyGameMode::StartGameIfAllReady()
{
    if (!HasAuthority()) return;

    // 1) 로비 상태 검사
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

    // 2) 목적지 맵 유효성
    if (!(StageMap.IsValid() || StageMap.ToSoftObjectPath().IsValid()))
    {
        UE_LOG(LogTemp, Error, TEXT("StageMap is not set or invalid."));
        return;
    }

    // 3) ★ 여기서 '단 한 번만' 색 확정 (ELobbyColor -> EEnemyColor)
    auto ToEnemyColor = [](ELobbyColor C)
        {
            switch (C)
            {
            case ELobbyColor::Red:   return EEnemyColor::Red;
            case ELobbyColor::Green: return EEnemyColor::Green;
            case ELobbyColor::Blue:  return EEnemyColor::Blue;
            default:                 return EEnemyColor::Red; // 폴백(프로젝트 규칙에 맞게)
            }
        };

    for (APlayerState* PSBase : GS->PlayerArray)
    {
        if (auto* PS = Cast<APlayerState_Real>(PSBase))
        {
            // 최종 1회 커밋
            PS->PlayerColor = ToEnemyColor(PS->SelectedColor);

            // (선택) 복제 즉시화를 원하면 유지, 아니면 주석처리 가능
            PS->ForceNetUpdate();

            UE_LOG(LogTemp, Display, TEXT("[PreTravel] %s Appear=%d Selected(ELobby)=%d -> PlayerColor(Enemy)=%d Skill=%d Ready=%d"),
                *GetNameSafe(PS), PS->AppearanceId, (int)PS->SelectedColor, (int)PS->PlayerColor, PS->SubskillId, (int)PS->bReady);
        }
    }

    // 4) 맵 전환 (Seamless)
    const FString MapPath = StageMap.GetLongPackageName();
    const FString Url = MapPath + TEXT("?listen");
    UE_LOG(LogTemp, Display, TEXT("All ready -> ServerTravel to %s"), *Url);

    GetWorld()->ServerTravel(Url);
}



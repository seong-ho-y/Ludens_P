// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "LobbyGameState.h"        
#include "PlayerState_Real.h" // 수정 완료



ALobbyGameMode::ALobbyGameMode()
{
    // S5에서 무중단 전환 예정이므로 미리 켜둠
    bUseSeamlessTravel = true;
    PlayerStateClass = APlayerState_Real::StaticClass();

    // 필요시 여기서 DefaultPawnClass / HUDClass / PlayerControllerClass 지정 가능
    // (이번 단계에선 BP에서 PlayerControllerClass를 지정하는 방식을 권장)
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

        // ★ 로비색 → 상성색 매핑(이 파일 전용 간단 람다)
        auto MapToEnemyColor = [](ELobbyColor C)
            {
                switch (C) {
                case ELobbyColor::Red:   return EEnemyColor::Red;
                case ELobbyColor::Green: return EEnemyColor::Green;
                case ELobbyColor::Blue:  return EEnemyColor::Blue;
                default:                 return EEnemyColor::Red; // 폴백
                }
            };

        // ★ ServerTravel 직전: 최종 1회 커밋 + 로그
        for (APlayerState* Base : GameState->PlayerArray)
        {
            if (auto* PS = Cast<APlayerState_Real>(Base)) // 캐스팅 대상 교체
            {
                PS->PlayerColor = MapToEnemyColor(PS->SelectedColor); //  최종색 확정
                PS->ForceNetUpdate(); // (선택) 안전하게 한 번 밀어줌

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


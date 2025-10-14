// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "LobbyGameState.h" 
#include "Kismet/GameplayStatics.h"
#include "LobbyPlayerController.h"
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

// LobbyGameMode.cpp의 StartGameIfAllReady() 함수 전체를 아래 코드로 교체하세요.
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

    for (APlayerState* PSBase : GS->PlayerArray)
    {
        if (APlayerState_Real* PSR = Cast<APlayerState_Real>(PSBase))
        {
            PSR->PlayerColor = PSR->SelectedColor;
            PSR->ForceNetUpdate();

            UE_LOG(LogTemp, Display, TEXT("[PreTravel] PS=%p Ap=%d Sel=%s -> PlayerColor=%s Name=%s"),
                PSR, PSR->AppearanceId, *ColorToStr(PSR->SelectedColor), *ColorToStr(PSR->PlayerColor), *PSR->GetPlayerName());
        }
    }

    const FString MapPath = StageMap.GetLongPackageName();
    const FString NextGameModePath = TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonGameMode.BP_FirstPersonGameMode_C");
    const FString URL = FString::Printf(TEXT("%s?listen?game=%s"), *MapPath, *NextGameModePath);

    UE_LOG(LogTemp, Display, TEXT("All ready -> ServerTravel to %s"), *URL);
    GetWorld()->ServerTravel(URL);
}

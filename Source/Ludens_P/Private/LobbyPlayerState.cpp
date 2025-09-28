// Fill out your copyright notice in the Description page of Project Settings.


// Source/Ludens_P/Private/LobbyPlayerState.cpp
#include "LobbyPlayerState.h"
#include "Net/UnrealNetwork.h"

ALobbyPlayerState::ALobbyPlayerState()
{
    bReplicates = true;
}

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ALobbyPlayerState, AppearanceId);
    DOREPLIFETIME(ALobbyPlayerState, PreviewColor);
    DOREPLIFETIME(ALobbyPlayerState, SelectedColor);
    DOREPLIFETIME(ALobbyPlayerState, SubskillId);
    DOREPLIFETIME(ALobbyPlayerState, bReady);
}

void ALobbyPlayerState::OnRep_Appearance() {

    UE_LOG(LogTemp, Display, TEXT("[LPS] OnRep_Appearance %d"), AppearanceId);
    OnAnyLobbyFieldChanged.Broadcast();
}
void ALobbyPlayerState::OnRep_PreviewColor()
{
    UE_LOG(LogTemp, Log, TEXT("[LPS] OnRep_PreviewColor %d"), (int)PreviewColor);
    OnAnyLobbyFieldChanged.Broadcast();
}
void ALobbyPlayerState::OnRep_SelectedColor() { OnAnyLobbyFieldChanged.Broadcast(); }
void ALobbyPlayerState::OnRep_Subskill() { OnAnyLobbyFieldChanged.Broadcast(); }
void ALobbyPlayerState::OnRep_Ready() { OnAnyLobbyFieldChanged.Broadcast(); }

void ALobbyPlayerState::NotifyAnyLobbyFieldChanged()
{
    OnAnyLobbyFieldChanged.Broadcast();
}


void ALobbyPlayerState::CopyProperties(APlayerState* PlayerState)
{
    Super::CopyProperties(PlayerState);

    if (ALobbyPlayerState* NewPS = Cast<ALobbyPlayerState>(PlayerState))
    {
        // 로비에서 확정/선택한 값들 모두 복사
        NewPS->AppearanceId = AppearanceId;
        NewPS->PreviewColor = PreviewColor;
        NewPS->SelectedColor = SelectedColor;
        NewPS->SubskillId = SubskillId;
        NewPS->bReady = bReady;

        // (선택) 우리가 쓰는 델리게이트/캐시 값은 런타임 재바인딩되므로 복사 불필요
    }
}

void ALobbyPlayerState::SeamlessTravelTo(APlayerState* NewPlayerState)
{
    // 엔진이 호출: 여기서도 한 번 더 보장해주면 안전
    Super::SeamlessTravelTo(NewPlayerState);
    CopyProperties(NewPlayerState);
}

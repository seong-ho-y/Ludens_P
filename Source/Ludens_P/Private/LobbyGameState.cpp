#include "LobbyGameState.h"
#include "Net/UnrealNetwork.h"

ALobbyGameState::ALobbyGameState()
{
    SetReplicates(true);
    ColorSlots.RedOwnerPlayerId = INDEX_NONE;
    ColorSlots.GreenOwnerPlayerId = INDEX_NONE;
    ColorSlots.BlueOwnerPlayerId = INDEX_NONE;
}


// 파일-전역(static) 헬퍼
static int32& SlotRefByColor(FLobbyColorSlots& Slots, EEnemyColor Color)
{
    switch (Color)
    {
    case EEnemyColor::Red:   return Slots.RedOwnerPlayerId;
    case EEnemyColor::Green: return Slots.GreenOwnerPlayerId;
    case EEnemyColor::Blue:  return Slots.BlueOwnerPlayerId;
    default:                 return Slots.RedOwnerPlayerId; // fallback
    }
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ALobbyGameState, ColorSlots);
    DOREPLIFETIME(ALobbyGameState, ReadyCount);   // ReplicatedUsing=OnRep_ReadyCount
    DOREPLIFETIME(ALobbyGameState, MaxPlayers);
}

bool ALobbyGameState::TryLockColor(int32 InPlayerId, EEnemyColor Color)
{
    // Enemy에는 None이 없으므로 유효성은 RGB 체크(또는 enum range 검사)
    int32& SlotOwner = SlotRefByColor(ColorSlots, Color);
    if (SlotOwner == INDEX_NONE)
    {
        SlotOwner = InPlayerId;
        return true;
    }
    return false;
}

void ALobbyGameState::UnlockColor(int32 InPlayerId, EEnemyColor Color)
{

    int32& SlotOwner = SlotRefByColor(ColorSlots, Color);
    if (SlotOwner == InPlayerId) SlotOwner = INDEX_NONE;
}

// ===== ReadyCount 동기화 =====

void ALobbyGameState::OnRep_ReadyCount()
{
    // 클라이언트에서 ReadyCount가 갱신될 때 위젯에 브로드캐스트
    OnLobbyReadyCountChanged.Broadcast(ReadyCount);
}

void ALobbyGameState::NotifyReadyCountChanged()
{
    // 서버(호스트 로컬 UI)는 OnRep가 안 울리므로 직접 브로드캐스트
    OnLobbyReadyCountChanged.Broadcast(ReadyCount);
}

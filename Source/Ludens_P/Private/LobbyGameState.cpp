#include "LobbyGameState.h"
#include "Net/UnrealNetwork.h"

ALobbyGameState::ALobbyGameState()
{
    SetReplicates(true);
    ColorSlots.RedOwnerPlayerId = INDEX_NONE;
    ColorSlots.GreenOwnerPlayerId = INDEX_NONE;
    ColorSlots.BlueOwnerPlayerId = INDEX_NONE;
}


// ����-����(static) ����
static int32& SlotRefByColor(FLobbyColorSlots& Slots, ELobbyColor Color)
{
    switch (Color)
    {
    case ELobbyColor::Red:   return Slots.RedOwnerPlayerId;
    case ELobbyColor::Green: return Slots.GreenOwnerPlayerId;
    case ELobbyColor::Blue:  return Slots.BlueOwnerPlayerId;
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

bool ALobbyGameState::TryLockColor(int32 InPlayerId, ELobbyColor Color)
{
    if (Color == ELobbyColor::None) return false;
    int32& SlotOwner = SlotRefByColor(ColorSlots, Color);
    if (SlotOwner == INDEX_NONE)
    {
        SlotOwner = InPlayerId;
        return true;
    }
    return false;
}

void ALobbyGameState::UnlockColor(int32 InPlayerId, ELobbyColor Color)
{
    if (Color == ELobbyColor::None) return;
    int32& SlotOwner = SlotRefByColor(ColorSlots, Color);
    if (SlotOwner == InPlayerId) SlotOwner = INDEX_NONE;
}

// ===== ReadyCount ����ȭ =====

void ALobbyGameState::OnRep_ReadyCount()
{
    // Ŭ���̾�Ʈ���� ReadyCount�� ���ŵ� �� ������ ��ε�ĳ��Ʈ
    OnLobbyReadyCountChanged.Broadcast(ReadyCount);
}

void ALobbyGameState::NotifyReadyCountChanged()
{
    // ����(ȣ��Ʈ ���� UI)�� OnRep�� �� �︮�Ƿ� ���� ��ε�ĳ��Ʈ
    OnLobbyReadyCountChanged.Broadcast(ReadyCount);
}

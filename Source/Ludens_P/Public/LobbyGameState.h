// LobbyGameState.h

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyTypes.h"
#include "LobbyGameState.generated.h"

// ��� Ŭ���̾�Ʈ(�� ȣ��Ʈ ���� UI)���� ReadyCount ���� �̺�Ʈ�� �����ϱ� ���� ��Ƽĳ��Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyReadyCountChanged, int32, NewReadyCount);

UCLASS()
class LUDENS_P_API ALobbyGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    ALobbyGameState();

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    FLobbyColorSlots ColorSlots;

    // �� ReadyCount�� OnRep���� ����
    UPROPERTY(ReplicatedUsing = OnRep_ReadyCount, BlueprintReadOnly, Category = "Lobby")
    int32 ReadyCount = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    int32 MaxPlayers = 3;

    // ȣ��Ʈ/Ŭ�� UI�� ���ε��ؼ� ���� �̺�Ʈ
    UPROPERTY(BlueprintAssignable, Category = "Lobby")
    FOnLobbyReadyCountChanged OnLobbyReadyCountChanged;

    // ����/Ŭ�� ���� ��ƿ: ��� �������� ��� (PlayerArray ���)
    UFUNCTION(BlueprintPure, Category = "Lobby")
    bool AreAllReady() const
    {
        const int32 Total = PlayerArray.Num();
        return (Total > 0 && ReadyCount == Total);
    }

    // ������ ReadyCount�� ������ "���" ȣ��Ʈ ���� UI�� ���ŵǰ� ��ε�ĳ��Ʈ(���� ���� ȣ�� ����)
    void NotifyReadyCountChanged();

    bool TryLockColor(int32 InPlayerId, ELobbyColor Color);
    void UnlockColor(int32 InPlayerId, ELobbyColor Color);

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // �� Ŭ���̾�Ʈ���� ReadyCount�� �����Ǿ� ���� �� ȣ��
    UFUNCTION()
    void OnRep_ReadyCount();
};

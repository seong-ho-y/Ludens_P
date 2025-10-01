// LobbyGameState.h

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LobbyTypes.h"
#include "LobbyGameState.generated.h"

// 모든 클라이언트(및 호스트 로컬 UI)에서 ReadyCount 변경 이벤트를 수신하기 위한 멀티캐스트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLobbyReadyCountChanged, int32, NewReadyCount);

UCLASS()
class LUDENS_P_API ALobbyGameState : public AGameStateBase
{
    GENERATED_BODY()

public:
    ALobbyGameState();

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    FLobbyColorSlots ColorSlots;

    // ★ ReadyCount를 OnRep으로 감시
    UPROPERTY(ReplicatedUsing = OnRep_ReadyCount, BlueprintReadOnly, Category = "Lobby")
    int32 ReadyCount = 0;

    UPROPERTY(Replicated, BlueprintReadOnly, Category = "Lobby")
    int32 MaxPlayers = 3;

    // 호스트/클라 UI가 바인딩해서 쓰는 이벤트
    UPROPERTY(BlueprintAssignable, Category = "Lobby")
    FOnLobbyReadyCountChanged OnLobbyReadyCountChanged;

    // 서버/클라 공용 유틸: 모두 레디인지 계산 (PlayerArray 기반)
    UFUNCTION(BlueprintPure, Category = "Lobby")
    bool AreAllReady() const
    {
        const int32 Total = PlayerArray.Num();
        return (Total > 0 && ReadyCount == Total);
    }

    // 서버가 ReadyCount를 변경한 "즉시" 호스트 로컬 UI도 갱신되게 브로드캐스트(서버 전용 호출 권장)
    void NotifyReadyCountChanged();

    bool TryLockColor(int32 InPlayerId, ELobbyColor Color);
    void UnlockColor(int32 InPlayerId, ELobbyColor Color);

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // ★ 클라이언트에서 ReadyCount가 복제되어 들어올 때 호출
    UFUNCTION()
    void OnRep_ReadyCount();
};

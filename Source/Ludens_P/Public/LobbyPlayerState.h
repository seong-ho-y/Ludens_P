
// Source/Ludens_P/Public/LobbyPlayerState.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LobbyTypes.h"                      
#include "LobbyPlayerState.generated.h"    

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyPSChanged);

UCLASS()
class LUDENS_P_API ALobbyPlayerState : public APlayerState
{
    GENERATED_BODY()

public:                                        // <-- public ���ǿ� UPROPERTY��!
    ALobbyPlayerState();

    UPROPERTY(ReplicatedUsing = OnRep_Appearance, BlueprintReadOnly, Category = "Lobby")
    int32 AppearanceId = -1;

    UPROPERTY(ReplicatedUsing = OnRep_PreviewColor, BlueprintReadOnly, Category = "Lobby")
    ELobbyColor PreviewColor = ELobbyColor::None;

    UPROPERTY(ReplicatedUsing = OnRep_SelectedColor, BlueprintReadOnly, Category = "Lobby")
    ELobbyColor SelectedColor = ELobbyColor::None;

    UPROPERTY(ReplicatedUsing = OnRep_Subskill, BlueprintReadOnly, Category = "Lobby")
    int32 SubskillId = -1;
    
    UPROPERTY(ReplicatedUsing = OnRep_Ready, BlueprintReadOnly, Category = "Lobby")
    bool bReady = false;

    UPROPERTY(BlueprintAssignable, Category = "Lobby")
    FOnLobbyPSChanged OnAnyLobbyFieldChanged;

    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void NotifyAnyLobbyFieldChanged(); // ����/ȣ��Ʈ ��� UI �ݿ���

    virtual void CopyProperties(APlayerState* PlayerState) override;
    virtual void SeamlessTravelTo(APlayerState* NewPlayerState) override;

protected:
    UFUNCTION() void OnRep_Appearance();
    UFUNCTION() void OnRep_PreviewColor();
    UFUNCTION() void OnRep_SelectedColor();
    UFUNCTION() void OnRep_Subskill();
    UFUNCTION() void OnRep_Ready();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};

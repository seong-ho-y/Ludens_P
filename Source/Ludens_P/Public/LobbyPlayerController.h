// LobbyPlayerController.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyTypes.h"
#include "LobbyPlayerController.generated.h"

// ---- Forward Declarations ----
class ALobbyPreviewRig;
class UTextureRenderTarget2D;
class UWBP_Lobby;
class APlayerState_Real;
class ALobbyGameState;
class APlayerState;

UCLASS()
class LUDENS_P_API ALobbyPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    // APlayerController
    virtual void BeginPlay() override;

    // 
    UFUNCTION(Server, Reliable) void ServerSetAppearance(int32 InAppearanceId);
    UFUNCTION(Server, Reliable) void ServerSetSubskill(int32 InSubskillId);
    UFUNCTION(Server, Reliable) void ServerSetPreviewColor(ELobbyColor InColor);
    UFUNCTION(Server, Reliable) void ServerReadyOn(ELobbyColor Requested);
    UFUNCTION(Server, Reliable) void ServerReadyOff();

    UFUNCTION(Server, Reliable)
    void ServerGameStart();


    // Ready�� Ÿ���� ����/���� �̴� ���� RT�� 1������ ĸó
    UFUNCTION(BlueprintCallable, Category = "Lobby|Preview")
    void CaptureMiniFor(APlayerState* OtherPS, bool bLeftSlot); // true = Left, false = Right

    void ClearMiniRT(bool bLeftSlot, const FLinearColor& Color = FLinearColor::White);

    UFUNCTION() void OnAnyPSChangedHandler();
    void RefreshMiniSlots();
    void BindAllPSDelegates();
    FTimerHandle PSBindTimer;

protected:
    // ---- ��Ÿ�� �����Ǵ� ������ ����(Ŭ�� ����/����) ----
    UPROPERTY() ALobbyPreviewRig* Rig_Self = nullptr;
    UPROPERTY() ALobbyPreviewRig* Rig_OtherL = nullptr;
    UPROPERTY() ALobbyPreviewRig* Rig_OtherR = nullptr;

    // ---- �����Ϳ��� ������ Ŭ����/����/RT �ڻ�(���Ž�) ----
    UPROPERTY(EditDefaultsOnly, Category = "Lobby|Preview")
    TSubclassOf<ALobbyPreviewRig> PreviewRigClass;

    UPROPERTY(EditDefaultsOnly, Category = "Lobby|UI")
    TSubclassOf<UWBP_Lobby> LobbyWidgetClass;

private:
    // ---- �κ� ���� �ν��Ͻ� ----
    UPROPERTY() UWBP_Lobby* LobbyWidget = nullptr;

    // ---- ������ ���� ���� & (���Ž� RT �輱), ������ ���� ���� ----
    void SpawnAndWirePreviewRigs();

    // ---- ���� ���� ----
    APlayerState_Real* GetLobbyPS() const;
    ALobbyGameState* GetLobbyGS() const;

    // ---- ���� RT: �� Ŭ���̾�Ʈ ���� (�ߺ� ����! private���� ��) ----
    UPROPERTY() UTextureRenderTarget2D* RT_SelfDyn = nullptr;
    UPROPERTY() UTextureRenderTarget2D* RT_OtherLDyn = nullptr;
    UPROPERTY() UTextureRenderTarget2D* RT_OtherRDyn = nullptr;

    // �� ��/�� ����� ���� ���� �� ��
    UPROPERTY() ALobbyPreviewRig* Rig_ThumbL = nullptr;
    UPROPERTY() ALobbyPreviewRig* Rig_ThumbR = nullptr;

    // �� ��/�� ����� ���׸� �����ϰ� ���� RT�� ���� ����
    void SpawnThumbRigs();

    // APlayerController
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


};

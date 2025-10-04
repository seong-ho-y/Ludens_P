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


    // Ready된 타인의 외형/색을 미니 슬롯 RT에 1프레임 캡처
    UFUNCTION(BlueprintCallable, Category = "Lobby|Preview")
    void CaptureMiniFor(APlayerState* OtherPS, bool bLeftSlot); // true = Left, false = Right

    void ClearMiniRT(bool bLeftSlot, const FLinearColor& Color = FLinearColor::White);

    UFUNCTION() void OnAnyPSChangedHandler();
    void RefreshMiniSlots();
    void BindAllPSDelegates();
    FTimerHandle PSBindTimer;

protected:
    // ---- 런타임 스폰되는 프리뷰 리그(클라 전용/비복제) ----
    UPROPERTY() ALobbyPreviewRig* Rig_Self = nullptr;
    UPROPERTY() ALobbyPreviewRig* Rig_OtherL = nullptr;
    UPROPERTY() ALobbyPreviewRig* Rig_OtherR = nullptr;

    // ---- 에디터에서 지정할 클래스/위젯/RT 자산(레거시) ----
    UPROPERTY(EditDefaultsOnly, Category = "Lobby|Preview")
    TSubclassOf<ALobbyPreviewRig> PreviewRigClass;

    UPROPERTY(EditDefaultsOnly, Category = "Lobby|UI")
    TSubclassOf<UWBP_Lobby> LobbyWidgetClass;

private:
    // ---- 로비 위젯 인스턴스 ----
    UPROPERTY() UWBP_Lobby* LobbyWidget = nullptr;

    // ---- 프리뷰 리그 스폰 & (레거시 RT 배선), 위젯에 참조 주입 ----
    void SpawnAndWirePreviewRigs();

    // ---- 내부 헬퍼 ----
    APlayerState_Real* GetLobbyPS() const;
    ALobbyGameState* GetLobbyGS() const;

    // ---- 동적 RT: 각 클라이언트 전용 (중복 금지! private에만 둠) ----
    UPROPERTY() UTextureRenderTarget2D* RT_SelfDyn = nullptr;
    UPROPERTY() UTextureRenderTarget2D* RT_OtherLDyn = nullptr;
    UPROPERTY() UTextureRenderTarget2D* RT_OtherRDyn = nullptr;

    // ★ 좌/우 썸네일 전용 리그 두 개
    UPROPERTY() ALobbyPreviewRig* Rig_ThumbL = nullptr;
    UPROPERTY() ALobbyPreviewRig* Rig_ThumbR = nullptr;

    // ★ 좌/우 썸네일 리그를 스폰하고 각자 RT에 고정 연결
    void SpawnThumbRigs();

    // APlayerController
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


};

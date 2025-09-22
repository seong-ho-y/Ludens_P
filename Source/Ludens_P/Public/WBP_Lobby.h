//WBP_Lobby.h

#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "LobbyTypes.h"
#include "WBP_Lobby.generated.h"

class ALobbyPreviewRig; class UButton; class UBorder;
class UTextBlock;
class ALobbyPlayerState;
class UWidget;


UCLASS()
class LUDENS_P_API UWBP_Lobby : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, Category = "Lobby|Preview")
    void InitPreviewRefs(ALobbyPreviewRig* InSelf, ALobbyPreviewRig* InOtherL, ALobbyPreviewRig* InOtherR);

    UFUNCTION(BlueprintImplementableEvent, Category = "Lobby|Preview")
    void OnPreviewRefsReady();

    UPROPERTY(BlueprintReadOnly, Category = "Lobby|Preview") ALobbyPreviewRig* Rig_Self = nullptr;
    UPROPERTY(BlueprintReadOnly, Category = "Lobby|Preview") ALobbyPreviewRig* Rig_OtherL = nullptr;
    UPROPERTY(BlueprintReadOnly, Category = "Lobby|Preview") ALobbyPreviewRig* Rig_OtherR = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby|Preview")
    float DragYawScale = 0.25f;

    UFUNCTION(BlueprintCallable, Category = "Lobby|Input") void BP_SetAppearance(int32 Id);
    UFUNCTION(BlueprintCallable, Category = "Lobby|Input") void BP_SetPreviewColor(ELobbyColor InColor);
    UFUNCTION(BlueprintCallable, Category = "Lobby|Input") void BP_SetSubskill(int32 Id);
    UFUNCTION(BlueprintCallable, Category = "Lobby|Input") void BP_ReadyOn();
    UFUNCTION(BlueprintCallable, Category = "Lobby|Input") void BP_ReadyOff();

    // WBP_Lobby.h  (public: ����)
    UFUNCTION(BlueprintCallable, Category = "Lobby|Preview")
    void SetPreviewRenderTargets(
        class UTextureRenderTarget2D* SelfRT,
        class UTextureRenderTarget2D* LeftRT,
        class UTextureRenderTarget2D* RightRT);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

    UPROPERTY(meta = (BindWidget)) UButton* Btn_A0;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_A1;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_A2;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_A3;

    UPROPERTY(meta = (BindWidget)) UBorder* Dim_A0;
    UPROPERTY(meta = (BindWidget)) UBorder* Dim_A1;
    UPROPERTY(meta = (BindWidget)) UBorder* Dim_A2;
    UPROPERTY(meta = (BindWidget)) UBorder* Dim_A3;

    UPROPERTY(meta = (BindWidget)) UBorder* Sel_A0;
    UPROPERTY(meta = (BindWidget)) UBorder* Sel_A1;
    UPROPERTY(meta = (BindWidget)) UBorder* Sel_A2;
    UPROPERTY(meta = (BindWidget)) UBorder* Sel_A3;

    // ----- RGB Buttons -----
    UPROPERTY(meta = (BindWidget)) UButton* Btn_ColorR;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_ColorG;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_ColorB;
    // ----- RGB Buttons -----

    // ----- Skill Buttons -----
    UPROPERTY(meta = (BindWidget)) UButton* Btn_S0;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_S1;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_S2;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_S3;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_S4;

    // (����) �ڱ� ���� ����� �ؽ�Ʈ
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* Txt_SkillSelf;
    // ----- Skill Buttons -----
    
    // ----- Ready (toggle) -----
    UPROPERTY(meta = (BindWidget)) UButton* Btn_Ready;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Txt_ReadyLabel;
    // ----- Ready (toggle) -----

    // ----- Host: GameStart -----
    UPROPERTY(meta = (BindWidgetOptional)) UButton* Btn_GameStart;
    // ----- Host: GameStart -----


    // ----- Other mini slots (color + READY only) -----
    UPROPERTY(meta = (BindWidgetOptional)) UBorder* Swatch_Color_OtherL;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* Txt_Ready_OtherL;

    UPROPERTY(meta = (BindWidgetOptional)) UBorder* Swatch_Color_OtherR;
    UPROPERTY(meta = (BindWidgetOptional)) UTextBlock* Txt_Ready_OtherR;



    UPROPERTY(meta = (BindWidget))   // �����̳� ���� �̸��� ��Ȯ�� ��ġ�ؾ� ��
    UBorder* Bdr_Self;

    // �巡�� ����
    bool bDragging = false;
    FVector2D LastMousePos = FVector2D::ZeroVector;

    // ��Ʈ�� ���� ���콺 ó��                   
    virtual FReply NativeOnMouseButtonDown(const FGeometry&, const FPointerEvent&) override;
    virtual FReply NativeOnMouseMove(const FGeometry&, const FPointerEvent&) override;
    virtual FReply NativeOnMouseButtonUp(const FGeometry&, const FPointerEvent&) override;
    virtual void   NativeOnMouseLeave(const FPointerEvent&) override;            // ĸó Ǯ�� ���
    virtual void   NativeOnMouseCaptureLost(const FCaptureLostEvent&) override;       // ĸó Ǯ�� ���

    void ReapplyUIOnlyInput();   // �巡�� �� ��� UI Only ������

    int32       LastAppearanceId = -1;                 // ��ȿ�� ������ 0 �̻��̹Ƿ�, -1�� �ʱ�ȭ
    bool        bLastReady = false;              // ���� Ready ����
    ELobbyColor LastPreviewColor = ELobbyColor::None;  // ���� �̸����� ��
    ELobbyColor LastSelectedColor = ELobbyColor::None;  // ���� Ȯ�� ��


private:
    void BindAppearanceButton(int32 Index, UButton* Btn);
    void SetDimVisible(int32 Index, bool bVisible);
    void UpdateAppearanceHighlight();

    UFUNCTION() void OnA0Pressed();  UFUNCTION() void OnA0Released();  UFUNCTION() void OnA0Clicked();
    UFUNCTION() void OnA1Pressed();  UFUNCTION() void OnA1Released();  UFUNCTION() void OnA1Clicked();
    UFUNCTION() void OnA2Pressed();  UFUNCTION() void OnA2Released();  UFUNCTION() void OnA2Clicked();
    UFUNCTION() void OnA3Pressed();  UFUNCTION() void OnA3Released();  UFUNCTION() void OnA3Clicked();

    UFUNCTION() void OnPSChanged();

    UPROPERTY() class ALobbyPlayerState* PS_Cached = nullptr;

    TArray<UBorder*> AllDim() const { return { Dim_A0, Dim_A1, Dim_A2, Dim_A3 }; }
    TArray<UBorder*> AllSel() const { return { Sel_A0, Sel_A1, Sel_A2, Sel_A3 }; }

    void BindColorButtons();

    UFUNCTION() void OnClick_ColorR();
    UFUNCTION() void OnClick_ColorG();
    UFUNCTION() void OnClick_ColorB();

    // PS ���� ���ε���
    FTimerHandle PSBindTimer;
    bool bPSBound = false;
    void TryBindPS();

    // ���� ��ų
    void BindSkillButtons();
    UFUNCTION() void OnClick_S0();
    UFUNCTION() void OnClick_S1();
    UFUNCTION() void OnClick_S2();
    UFUNCTION() void OnClick_S3();
    UFUNCTION() void OnClick_S4();

    // ----- Ready (toggle) -----
    void BindReadyToggle();
    UFUNCTION() void OnClick_ReadyToggle();

    // Ready ��� UI/Ȱ�� ����
    void UpdateReadyToggleUI();
    bool CanReadyNow() const;

    // ----- Host: GameStart -----
    void BindGameStartButton();
    UFUNCTION() void OnClick_GameStart();
    void UpdateGameStartUI();
    bool IsHostLocal() const;

    UFUNCTION() void OnGSReadyCountChanged(int32 NewReadyCount);

    // Ÿ�� �̴Ͻ��� ����
    void RefreshOtherSlots();
    void RebindOtherPSDelegates();
    UFUNCTION() void OnAnyPSChanged();

    // Ÿ�� PS �ٿ�� ����(�ߺ� ����)
    TSet<TWeakObjectPtr<ALobbyPlayerState>> BoundOtherPS;


    UPROPERTY(meta = (BindWidget))          class UImage* Img_Self = nullptr;
    UPROPERTY(meta = (BindWidgetOptional))  class UImage* Img_OtherL = nullptr;
    UPROPERTY(meta = (BindWidgetOptional))  class UImage* Img_OtherR = nullptr;

};

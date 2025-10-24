//WBP_Lobby.cpp


#include "WBP_Lobby.h"
#include "LobbyPreviewRig.h"
#include "LobbyPlayerController.h"
#include "PlayerState_Real.h" // ���� �Ϸ�
#include "LobbyGameState.h"  
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Framework/Application/SlateApplication.h"
#include "InputCoreTypes.h" // EKeys
#include "Components/Image.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameStateBase.h"
#include "Ludens_P/EEnemyColor.h"
#include "LobbyTypes.h"

int32 UWBP_Lobby::ColorToIdx(EEnemyColor Col) const
{
    switch (Col)
    {
    case EEnemyColor::Red:   return 0;
    case EEnemyColor::Green: return 1;
    case EEnemyColor::Blue:  return 2;
    default:                 return 0;
    }
}


void UWBP_Lobby::InitPreviewRefs(ALobbyPreviewRig* InSelf, ALobbyPreviewRig* InOtherL, ALobbyPreviewRig* InOtherR)
{
    Rig_Self = InSelf;
    Rig_OtherL = InOtherL;
    Rig_OtherR = InOtherR;
    OnPreviewRefsReady();
}

void UWBP_Lobby::BP_SetAppearance(int32 Id)
{
    if (auto PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
        PC->ServerSetAppearance(Id);
}
void UWBP_Lobby::BP_SetPreviewColor(EEnemyColor InColor)
{
    CurrentColorCache = InColor; // ���� ���� ĳ�� ������Ʈ

    if (auto* PC = GetOwningPlayer<ALobbyPlayerController>())
    {
        PC->ServerSetPreviewColor(InColor); // ��Ʈ�ѷ��� EnemyColor ���� RPC�� ������ �� ����
    }
}

void UWBP_Lobby::BP_SetSubskill(int32 Id)
{
    if (auto PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
        PC->ServerSetSubskill(Id);
}


void UWBP_Lobby::BP_ReadyOn()
{
    // 1) �⺻�� ĳ�ÿ� �� EnemyColor�� ���
    EEnemyColor FinalColor = CurrentColorCache;

    // 2) ������: ĳ�ð� �ʱⰪ/��ȿ���� �ʴٰ� �ǴܵǸ� PS�� PreviewColor�� �о� ��ȯ
    if (auto* PC = GetOwningPlayer<ALobbyPlayerController>())
    {
        if (auto* PS = PC->GetLobbyPS())
        {
            if (PS->PreviewColor == EEnemyColor::Red
                || PS->PreviewColor == EEnemyColor::Green
                || PS->PreviewColor == EEnemyColor::Blue)
            {
                FinalColor = PS->PreviewColor;
            }
        }

        // 3) ���������� EnemyColor �ϳ��� �ѱ��
        PC->ServerReadyOn(FinalColor);
    }

    // UI ���/��ư ��Ȱ�� �� ��ó����
}

void UWBP_Lobby::BP_ReadyOff()
{
    if (auto PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
        PC->ServerReadyOff();
}

void UWBP_Lobby::NativeConstruct()
{
    Super::NativeConstruct();

    // PS ��ȭ ���� ���ε�
    if (APlayerState_Real* PSR = GetOwningPlayer() ? GetOwningPlayer()->GetPlayerState<APlayerState_Real>() : nullptr) {

    }
    UpdatePortraitFromPS(); // �ʱ� 1ȸ


    // ��Ʈ ������ ��Ŀ���� ���� �� �ְ�(���콺 ĸó ������ ��)
    SetIsFocusable(true);

    // debug log
    UE_LOG(LogTemp, Warning, TEXT("[UI] Construct: Focusable=1 Widget=%s"), *GetName());


    BindAppearanceButton(0, Btn_A0);
    BindAppearanceButton(1, Btn_A1);
    BindAppearanceButton(2, Btn_A2);
    BindAppearanceButton(3, Btn_A3);

    // RGB ��ư ���ε�
    BindColorButtons();

    // ���� ��ų ��ư ���ε�
    BindSkillButtons();

    // ���� & ���� ���� ��ư ���ε�
    BindReadyToggle();
    BindGameStartButton();

    // PlayerState ��������Ʈ ���� (����)
    if (auto PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
    {
        PS_Cached = PC->GetPlayerState<APlayerState_Real>();
        if (PS_Cached)
        {
            PS_Cached->OnAnyLobbyFieldChanged.AddDynamic(this, &UWBP_Lobby::OnPSChanged);
            bPSBound = true;

            UpdateReadyToggleUI();
            UpdateGameStartUI();
        }
    }
    // �� PS�� ���� ������ 0.25�� �������� ��õ�
    if (!bPSBound && GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            PSBindTimer, this, &UWBP_Lobby::TryBindPS, 0.25f, true, 0.f);
    }

    // �� GameState �̺�Ʈ ���ε� + �ʱ� ����
    if (auto GS = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr)
    {
        GS->OnLobbyReadyCountChanged.AddDynamic(this, &UWBP_Lobby::OnGSReadyCountChanged);
        UpdateGameStartUI(); // ���� ���·� �ʱ� �ݿ�

        RebindOtherPSDelegates();
        RefreshOtherSlots();
    }

}


void UWBP_Lobby::BindAppearanceButton(int32 Index, UButton* Btn)
{
    if (!Btn) return;
    switch (Index)
    {
    case 0: Btn->OnPressed.AddDynamic(this, &UWBP_Lobby::OnA0Pressed);
        Btn->OnReleased.AddDynamic(this, &UWBP_Lobby::OnA0Released);
        Btn->OnClicked.AddDynamic(this, &UWBP_Lobby::OnA0Clicked); break;
    case 1: Btn->OnPressed.AddDynamic(this, &UWBP_Lobby::OnA1Pressed);
        Btn->OnReleased.AddDynamic(this, &UWBP_Lobby::OnA1Released);
        Btn->OnClicked.AddDynamic(this, &UWBP_Lobby::OnA1Clicked); break;
    case 2: Btn->OnPressed.AddDynamic(this, &UWBP_Lobby::OnA2Pressed);
        Btn->OnReleased.AddDynamic(this, &UWBP_Lobby::OnA2Released);
        Btn->OnClicked.AddDynamic(this, &UWBP_Lobby::OnA2Clicked); break;
    case 3: Btn->OnPressed.AddDynamic(this, &UWBP_Lobby::OnA3Pressed);
        Btn->OnReleased.AddDynamic(this, &UWBP_Lobby::OnA3Released);
        Btn->OnClicked.AddDynamic(this, &UWBP_Lobby::OnA3Clicked); break;
    }
}






void UWBP_Lobby::OnPSChanged()
{
    UpdatePortraitFromPS();

    // ���� UI ���� ��ƾ ����
    UpdateReadyToggleUI();
    UpdateGameStartUI();
    RefreshOtherSlots();
    
    /*
    // �߾� ������ �ݿ� (���� '���� �ٲ� ���'���� ����)
    if (!Rig_Self || !PS_Cached)
    {
        return;
    }
    

    const int32      NewAppearanceId = PS_Cached->AppearanceId;
    const bool       bReady = PS_Cached->bReady;
    const EEnemyColor PreviewC = PS_Cached->PreviewColor;
    const EEnemyColor SelectedC = PS_Cached->SelectedColor;

    // (a) ���� ���� �ÿ��� ���� + ���� ����
    if (NewAppearanceId >= 0 && NewAppearanceId != LastAppearanceId)
    {
        Rig_Self->SetAppearance(NewAppearanceId);
        Rig_Self->SnapYawToZero();           // �� ������ ������ �ٲ� '�� ����'���� ����
        LastAppearanceId = NewAppearanceId;
    }

    // ���� �ݿ�: Ready ��/�� �б� + '���� �ٲ���� ����' ����
    if (!bReady)
    {
        if (PreviewC != LastPreviewColor)
        {
            Rig_Self->SetPreviewColor(PreviewC);
            LastPreviewColor = PreviewC;
        }
    }
    else
    {
        if (SelectedC != LastSelectedColor)
        {
            Rig_Self->SetSelectedColor(SelectedC);
            LastSelectedColor = SelectedC;
        }
    }

    // (b) Ready ��� ����: ON �� ���� ���� + �巡�� ���
    if (bReady != bLastReady)
    {
        if (bReady)
        {
            Rig_Self->SnapYawToZero();       // �� Ready�� ���� ���� ���� ����
            bDragging = false;               // �巡�� ��� �ߴ�(���)
        }
        else
        {
            // Ready ����: ���� ������, ������ ���� ���� �� ����ڰ� ���� ���� ����
        }

        bLastReady = bReady;
    }
    */
}




void UWBP_Lobby::BindColorButtons()
{
    if (Btn_ColorR) Btn_ColorR->OnClicked.AddDynamic(this, &UWBP_Lobby::OnClick_ColorR);
    if (Btn_ColorG) Btn_ColorG->OnClicked.AddDynamic(this, &UWBP_Lobby::OnClick_ColorG);
    if (Btn_ColorB) Btn_ColorB->OnClicked.AddDynamic(this, &UWBP_Lobby::OnClick_ColorB);
}

// ��
void UWBP_Lobby::OnClick_ColorR() { if (!PS_Cached || PS_Cached->bReady) return; BP_SetPreviewColor(EEnemyColor::Red); }
void UWBP_Lobby::OnClick_ColorG() { if (!PS_Cached || PS_Cached->bReady) return; BP_SetPreviewColor(EEnemyColor::Green); }
void UWBP_Lobby::OnClick_ColorB() { if (!PS_Cached || PS_Cached->bReady) return; BP_SetPreviewColor(EEnemyColor::Blue); }

void UWBP_Lobby::NativeDestruct()
{
    UWorld* W = GetWorld();

    if (W)
    {
        if (auto GS = W->GetGameState<ALobbyGameState>())
        {
            GS->OnLobbyReadyCountChanged.RemoveDynamic(this, &UWBP_Lobby::OnGSReadyCountChanged);
        }

        if (PSBindTimer.IsValid())
        {
            W->GetTimerManager().ClearTimer(PSBindTimer);
            PSBindTimer.Invalidate(); // (����) �ڵ� ��ȿȭ
        }
    }

    if (PS_Cached)
    {
        PS_Cached->OnAnyLobbyFieldChanged.RemoveDynamic(this, &UWBP_Lobby::OnPSChanged);
        PS_Cached = nullptr;
    }

    for (auto& Weak : BoundOtherPS)
        if (auto* PS = Weak.Get())
            PS->OnAnyLobbyFieldChanged.RemoveDynamic(this, &UWBP_Lobby::OnAnyPSChanged);
    BoundOtherPS.Empty();

    Super::NativeDestruct();
}


void UWBP_Lobby::TryBindPS()
{
    if (bPSBound) return;

    if (auto PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
    {
        if (auto PS = PC->GetPlayerState<APlayerState_Real>())
        {
            PS_Cached = PS;
            PS_Cached->OnAnyLobbyFieldChanged.AddDynamic(this, &UWBP_Lobby::OnPSChanged);
            bPSBound = true;


            UpdateReadyToggleUI();
            UpdateGameStartUI();

            // Ÿ�̸� ����
            if (GetWorld())
            {
                GetWorld()->GetTimerManager().ClearTimer(PSBindTimer);
            }
        }
    }
}


// ----- Skill Buttons -----

void UWBP_Lobby::BindSkillButtons()
{
    if (Btn_S0) Btn_S0->OnClicked.AddDynamic(this, &UWBP_Lobby::OnClick_S0);
    if (Btn_S1) Btn_S1->OnClicked.AddDynamic(this, &UWBP_Lobby::OnClick_S1);
    if (Btn_S2) Btn_S2->OnClicked.AddDynamic(this, &UWBP_Lobby::OnClick_S2);
    if (Btn_S3) Btn_S3->OnClicked.AddDynamic(this, &UWBP_Lobby::OnClick_S3);
}

void UWBP_Lobby::OnClick_S0() { BP_SetSubskill(0); }
void UWBP_Lobby::OnClick_S1() { BP_SetSubskill(1); }
void UWBP_Lobby::OnClick_S2() { BP_SetSubskill(2); }
void UWBP_Lobby::OnClick_S3() { BP_SetSubskill(3); }
void UWBP_Lobby::OnClick_S4() { BP_SetSubskill(4); }

// ----- Skill Buttons -----


// ----- Ready Buttons -----

void UWBP_Lobby::BindReadyToggle()
{
    if (Btn_Ready) Btn_Ready->OnClicked.AddDynamic(this, &UWBP_Lobby::OnClick_ReadyToggle);
}

void UWBP_Lobby::OnClick_ReadyToggle()
{
    if (!PS_Cached) return;


    if (PS_Cached->bReady)
    {
        BP_ReadyOff();
    }
    else
    {
        if (CanReadyNow())
        {
            BP_ReadyOn();
        }
        else
        {
            // (����) S3���� �佺Ʈ/����� "���� ������" �ǵ��
        }
    }
}

bool UWBP_Lobby::CanReadyNow() const
{
    if (!PS_Cached) return false;

    const bool bHasColor = (PS_Cached->PreviewColor == EEnemyColor::Red || PS_Cached->PreviewColor == EEnemyColor::Green || PS_Cached->PreviewColor == EEnemyColor::Blue);
    const bool bHasAppearance = (PS_Cached->AppearanceId >= 0);
    const bool bHasSkill = (PS_Cached->SubskillId >= 0);
    return bHasColor && bHasAppearance && bHasSkill && !PS_Cached->bReady;
}


void UWBP_Lobby::UpdateReadyToggleUI()
{
    const bool bReady = (PS_Cached && PS_Cached->bReady);
    const bool bCan = CanReadyNow() || bReady; // �̹� Ready�� �׻� Ȱ��

    if (Txt_ReadyLabel)
        Txt_ReadyLabel->SetText(bReady ? FText::FromString(TEXT("Ready"))
            : FText::FromString(TEXT("UnReady")));

    if (Btn_Ready)
    {
        Btn_Ready->SetIsEnabled(bCan);
        Btn_Ready->SetRenderOpacity(bCan ? 1.f : 0.5f); // ��Ȱ�� �� ��Ӱ�
    }

    // Ready�� ���� ���(��/��ų ��ư ��Ȱ��)
    const bool bEnableEdit = !bReady;
    if (Btn_ColorR) Btn_ColorR->SetIsEnabled(bEnableEdit);
    if (Btn_ColorG) Btn_ColorG->SetIsEnabled(bEnableEdit);
    if (Btn_ColorB) Btn_ColorB->SetIsEnabled(bEnableEdit);
    if (Btn_S0)     Btn_S0->SetIsEnabled(bEnableEdit);
    if (Btn_S1)     Btn_S1->SetIsEnabled(bEnableEdit);
    if (Btn_S2)     Btn_S2->SetIsEnabled(bEnableEdit);
    if (Btn_S3)     Btn_S3->SetIsEnabled(bEnableEdit);

    // �� ���� ��ư�� �Բ� ���
    if (Btn_A0) Btn_A0->SetIsEnabled(bEnableEdit);
    if (Btn_A1) Btn_A1->SetIsEnabled(bEnableEdit);
    if (Btn_A2) Btn_A2->SetIsEnabled(bEnableEdit);
    if (Btn_A3) Btn_A3->SetIsEnabled(bEnableEdit);
}

bool UWBP_Lobby::IsHostLocal() const
{
    if (auto PC = Cast<APlayerController>(GetOwningPlayer()))
    {
        // Listen ������ ���� ��Ʈ�ѷ��� true
        return PC->HasAuthority() && PC->IsLocalController();
    }
    return false;
}

void UWBP_Lobby::BindGameStartButton()
{
    if (Btn_GameStart)
    {
        Btn_GameStart->OnClicked.AddDynamic(this, &UWBP_Lobby::OnClick_GameStart);
        // ȣ��Ʈ�� �ƴϸ� ���߱�
        Btn_GameStart->SetVisibility(IsHostLocal() ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

void UWBP_Lobby::OnClick_GameStart()
{
    if (!IsHostLocal()) return;
    if (auto* PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
    {
        PC->ServerGameStart();
    }
}


void UWBP_Lobby::UpdateGameStartUI()
{
    if (!Btn_GameStart) return;

    if (!IsHostLocal())
    {
        Btn_GameStart->SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    // ��� �÷��̾� Ready ���� Ȯ��
    bool bAllReady = false;
    if (auto GS = GetWorld() ? GetWorld()->GetGameState<class ALobbyGameState>() : nullptr)
    {
        const int32 Total = GS->PlayerArray.Num();
        const int32 Ready = GS->ReadyCount;        // ALobbyGameState�� �̹� ����
        bAllReady = (Total > 0 && Ready == Total);
    }

    Btn_GameStart->SetIsEnabled(bAllReady);
    Btn_GameStart->SetRenderOpacity(bAllReady ? 1.f : 0.5f);
}

void UWBP_Lobby::OnGSReadyCountChanged(int32 /*NewReadyCount*/)
{
    UpdateGameStartUI();

    RebindOtherPSDelegates();   // ���� ����/���� PS �ݿ�
    RefreshOtherSlots();
}

// ---- [Other mini slots] helpers ----
static FLinearColor ColorForSlot(const APlayerState_Real* PS)
{
    if (!PS) return FLinearColor(0, 0, 0, 0);
    const EEnemyColor C = PS->bReady ? PS->SelectedColor : PS->PreviewColor;
    switch (C)
    {
    case EEnemyColor::Red:   return FLinearColor(1, 0, 0, 1);
    case EEnemyColor::Green: return FLinearColor(0, 1, 0, 1);
    case EEnemyColor::Blue:  return FLinearColor(0, 0, 1, 1);
    default:                 return FLinearColor(0, 0, 0, 0);
    }
}

/*
void UWBP_Lobby::RefreshOtherSlots()
{
    // GS/PC Ȯ��
    auto* GS = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
    auto* PC = GetOwningPlayer<ALobbyPlayerController>();   // �� ��Ʈ�ѷ��� �츮 Ÿ������
    if (!GS || !PC) return;

    // �� PS
    auto* SelfPS = PC->GetPlayerState<APlayerState_Real>();

    // Ÿ�� ���(�ִ� 2��)
    TArray<APlayerState_Real*> Others;
    Others.Reserve(2);
    for (APlayerState* Base : GS->PlayerArray)
    {
        if (auto* LPS = Cast<APlayerState_Real>(Base))
        {
            if (LPS != SelfPS) Others.Add(LPS);
        }
    }

    // ��/�� ������ ����(���� PlayerId�� ����)
    Others.Sort([](const APlayerState_Real& A, const APlayerState_Real& B)
        {
            return A.GetPlayerId() < B.GetPlayerId();
        });

    // ��/READY �� �ݿ�(���� ���� ����)
    auto FillMini = [](UBorder* Swatch, UTextBlock* TxtReady, const APlayerState_Real* PS)
        {
            if (Swatch)   Swatch->SetBrushColor(ColorForSlot(PS));
            if (TxtReady) TxtReady->SetText((PS && PS->bReady) ? FText::FromString(TEXT("READY")) : FText());
        };

    APlayerState_Real* L = Others.IsValidIndex(0) ? Others[0] : nullptr;
    APlayerState_Real* R = Others.IsValidIndex(1) ? Others[1] : nullptr;

    // ��/READY ���� ����ó��
    FillMini(Swatch_Color_OtherL, Txt_Ready_OtherL, L);
    FillMini(Swatch_Color_OtherR, Txt_Ready_OtherR, R);

    // �� Ready �� ��븸 ����� ĸó (���� RT�� 1������)
    // ����� ó��
    if (L && L->bReady) { PC->CaptureMiniFor(L, true); }
    else { PC->ClearMiniRT(true); }      // �� Unready/���� �� ����

    if (R && R->bReady) { PC->CaptureMiniFor(R, false); }
    else { PC->ClearMiniRT(/false); }     // �� Unready/���� �� ����

}
*/


void UWBP_Lobby::RebindOtherPSDelegates()
{
    auto* GS = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
    auto* PC = GetOwningPlayer();
    if (!GS || !PC) return;

    auto* SelfPS = PC->GetPlayerState<APlayerState_Real>();

    // ���� ���ε� ����
    for (auto& Weak : BoundOtherPS)
        if (auto* PS = Weak.Get())
            PS->OnAnyLobbyFieldChanged.RemoveDynamic(this, &UWBP_Lobby::OnAnyPSChanged);
    BoundOtherPS.Empty();

    // �� ���ε�
    for (APlayerState* Base : GS->PlayerArray)
        if (auto* LPS = Cast<APlayerState_Real>(Base))
            if (LPS != SelfPS)
            {
                LPS->OnAnyLobbyFieldChanged.AddDynamic(this, &UWBP_Lobby::OnAnyPSChanged);
                BoundOtherPS.Add(LPS);
            }
}

void UWBP_Lobby::OnAnyPSChanged()
{
    RefreshOtherSlots();
}


void UWBP_Lobby::ReapplyUIOnlyInput()
{
    if (APlayerController* PC = GetOwningPlayer())
    {
        FInputModeUIOnly Mode;
        Mode.SetWidgetToFocus(TakeWidget());
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        PC->SetInputMode(Mode);
        PC->bShowMouseCursor = true;
        PC->SetIgnoreMoveInput(true);
        PC->SetIgnoreLookInput(true);

        UE_LOG(LogTemp, Warning, TEXT("[UI] ReapplyUIOnly: IgnoreMove=%d IgnoreLook=%d Focus=%d Captor=%d"),
            PC->IsMoveInputIgnored(), PC->IsLookInputIgnored(),
            HasAnyUserFocus() ? 1 : 0,
            FSlateApplication::Get().HasAnyMouseCaptor() ? 1 : 0);
    }
}

FReply UWBP_Lobby::NativeOnMouseButtonDown(const FGeometry& Geo, const FPointerEvent& E)
{
    const FVector2D Screen = E.GetScreenSpacePosition();
    const bool bOverSelf = (Bdr_Self && Bdr_Self->GetCachedGeometry().IsUnderLocation(Screen));
    UE_LOG(LogTemp, Warning, TEXT("[UI] DOWN btn=%s overSelf=%d ready=%d focus=%d captor=%d"),
        *E.GetEffectingButton().ToString(), bOverSelf ? 1 : 0,
        (PS_Cached ? (PS_Cached->bReady ? 1 : 0) : -1),
        HasAnyUserFocus() ? 1 : 0,
        FSlateApplication::Get().HasAnyMouseCaptor() ? 1 : 0);

    if (!Rig_Self || !PS_Cached || PS_Cached->bReady) return Super::NativeOnMouseButtonDown(Geo, E);
    if (E.GetEffectingButton() != EKeys::LeftMouseButton) return Super::NativeOnMouseButtonDown(Geo, E);
    if (!bOverSelf) return Super::NativeOnMouseButtonDown(Geo, E);

    bDragging = true;
    LastMousePos = Screen;
    ReapplyUIOnlyInput();
    return FReply::Handled().CaptureMouse(Bdr_Self ? Bdr_Self->TakeWidget() : TakeWidget());
}

FReply UWBP_Lobby::NativeOnMouseMove(const FGeometry& Geo, const FPointerEvent& E)
{
    UE_LOG(LogTemp, Warning, TEXT("[UI] MOVE drag=%d captor=%d focus=%d"),
        bDragging ? 1 : 0,
        FSlateApplication::Get().HasAnyMouseCaptor() ? 1 : 0,
        HasAnyUserFocus() ? 1 : 0);

    if (bDragging && Rig_Self && PS_Cached && !PS_Cached->bReady)
    {
        const FVector2D Now = E.GetScreenSpacePosition();
        const float DeltaX = Now.X - LastMousePos.X;
        LastMousePos = Now;
        Rig_Self->AddYaw(DeltaX * DragYawScale);
        ReapplyUIOnlyInput();
        return FReply::Handled();
    }
    return Super::NativeOnMouseMove(Geo, E);
}

FReply UWBP_Lobby::NativeOnMouseButtonUp(const FGeometry& Geo, const FPointerEvent& E)
{
    UE_LOG(LogTemp, Warning, TEXT("[UI] UP drag=%d captor=%d"), bDragging ? 1 : 0,
        FSlateApplication::Get().HasAnyMouseCaptor() ? 1 : 0);

    if (bDragging)
    {
        bDragging = false;
        FReply R = FReply::Handled().ReleaseMouseCapture();
        ReapplyUIOnlyInput();
        return R;
    }
    return Super::NativeOnMouseButtonUp(Geo, E);
}

void UWBP_Lobby::NativeOnMouseLeave(const FPointerEvent& E)
{
    UE_LOG(LogTemp, Warning, TEXT("[UI] LEAVE drag=%d captor=%d focus=%d"), bDragging ? 1 : 0,
        FSlateApplication::Get().HasAnyMouseCaptor() ? 1 : 0,
        HasAnyUserFocus() ? 1 : 0);

    bDragging = false;
    FSlateApplication::Get().ReleaseAllPointerCapture();
    Super::NativeOnMouseLeave(E);
}

void UWBP_Lobby::NativeOnMouseCaptureLost(const FCaptureLostEvent& E)
{
    UE_LOG(LogTemp, Warning, TEXT("[UI] CAPTURE LOST (drag=%d)"), bDragging ? 1 : 0);
    bDragging = false;
    Super::NativeOnMouseCaptureLost(E);
}

static void SetImageRT(UImage* Img, UTextureRenderTarget2D* RT)
{
    if (!Img || !RT) return;
    FSlateBrush Brush = Img->Brush;    // ���� ��Ÿ�� ����
    Brush.SetResourceObject(RT);       // �� RT�� �� ������� ����
    Img->SetBrush(Brush);
}


void UWBP_Lobby::SetPreviewRenderTargets(UTextureRenderTarget2D* SelfRT,
    UTextureRenderTarget2D* LeftRT,
    UTextureRenderTarget2D* RightRT)
{
    SetImageRT(Img_Self, SelfRT);
    SetImageRT(Img_OtherL, LeftRT);
    SetImageRT(Img_OtherR, RightRT);

}


void UWBP_Lobby::UpdatePortraitFromPS() {
    if (!ImgPortrait) return;

    APlayerState_Real* PSR = GetOwningPlayer() ? GetOwningPlayer()->GetPlayerState<APlayerState_Real>() : nullptr;
    if (!PSR) return;

    const int32 ApId = FMath::Clamp(PSR->AppearanceId, 0, 3);

    // Ready��� Ȯ����(SelectedColor), ���� ���̸� �������(PreviewColor)
    const EEnemyColor UseColor = PSR->bReady ? PSR->SelectedColor : PSR->PreviewColor;
    const int32 ColIdx = ColorToIdx(UseColor);

    if (!PortraitByAppearance.IsValidIndex(ApId)) return;

    TSoftObjectPtr<UTexture2D> SoftTex;
    const FPortraitSet& Set = PortraitByAppearance[ApId];
    if (ColIdx == 0) SoftTex = Set.C;
    else if (ColIdx == 1) SoftTex = Set.M;
    else                  SoftTex = Set.Y;

    if (SoftTex.IsNull()) return;

    // �ʿ� �� ���� �ε�(12�� �Ը�� OK)
    UTexture2D* Tex = SoftTex.LoadSynchronous();
    ImgPortrait->SetBrushFromTexture(Tex, /*bMatchSize=*/true);
}

void UWBP_Lobby::RefreshOtherSlots()
{
}

// ---- Appearance Button Handlers ----
void UWBP_Lobby::OnA0Pressed() {}
void UWBP_Lobby::OnA0Released() {}
void UWBP_Lobby::OnA0Clicked() { BP_SetAppearance(0); }

void UWBP_Lobby::OnA1Pressed() {}
void UWBP_Lobby::OnA1Released() {}
void UWBP_Lobby::OnA1Clicked() { BP_SetAppearance(1); }

void UWBP_Lobby::OnA2Pressed() {}
void UWBP_Lobby::OnA2Released() {}
void UWBP_Lobby::OnA2Clicked() { BP_SetAppearance(2); }

void UWBP_Lobby::OnA3Pressed() {}
void UWBP_Lobby::OnA3Released() {}
void UWBP_Lobby::OnA3Clicked() { BP_SetAppearance(3); }
// ---- /Appearance Button Handlers ----
    
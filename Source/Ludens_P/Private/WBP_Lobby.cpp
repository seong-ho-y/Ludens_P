//WBP_Lobby.cpp


#include "WBP_Lobby.h"
#include "LobbyPreviewRig.h"
#include "LobbyPlayerController.h"
#include "LobbyPlayerState.h"
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
void UWBP_Lobby::BP_SetPreviewColor(ELobbyColor InColor)
{
    if (auto PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
        PC->ServerSetPreviewColor(InColor);
}
void UWBP_Lobby::BP_SetSubskill(int32 Id)
{
    if (auto PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
        PC->ServerSetSubskill(Id);
}
void UWBP_Lobby::BP_ReadyOn()
{
    if (auto PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
        if (auto PS = PC->GetPlayerState<ALobbyPlayerState>())
            PC->ServerReadyOn(PS->PreviewColor);
}
void UWBP_Lobby::BP_ReadyOff()
{
    if (auto PC = Cast<ALobbyPlayerController>(GetOwningPlayer()))
        PC->ServerReadyOff();
}

void UWBP_Lobby::NativeConstruct()
{
    Super::NativeConstruct();

    // ��Ʈ ������ ��Ŀ���� ���� �� �ְ�(���콺 ĸó ������ ��)
    SetIsFocusable(true);

    // debug log
    UE_LOG(LogTemp, Warning, TEXT("[UI] Construct: Focusable=1 Widget=%s"), *GetName());

    // ���� ����� �ʱ�ȭ/���ε�
    for (UBorder* B : AllDim()) if (B) B->SetVisibility(ESlateVisibility::Hidden);
    for (UBorder* B : AllSel()) if (B) B->SetVisibility(ESlateVisibility::Hidden);
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
        PS_Cached = PC->GetPlayerState<ALobbyPlayerState>();
        if (PS_Cached)
        {
            PS_Cached->OnAnyLobbyFieldChanged.AddDynamic(this, &UWBP_Lobby::OnPSChanged);
            bPSBound = true;
            UpdateAppearanceHighlight();

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

void UWBP_Lobby::SetDimVisible(int32 Index, bool bVisible)
{
    TArray<UBorder*> D = AllDim();
    if (D.IsValidIndex(Index) && D[Index])
        D[Index]->SetVisibility(bVisible ? ESlateVisibility::SelfHitTestInvisible
            : ESlateVisibility::Hidden);
}

void UWBP_Lobby::OnA0Pressed() { SetDimVisible(0, true); }
void UWBP_Lobby::OnA0Released() { SetDimVisible(0, false); }

void UWBP_Lobby::OnA1Pressed() { SetDimVisible(1, true); }
void UWBP_Lobby::OnA1Released() { SetDimVisible(1, false); }

void UWBP_Lobby::OnA2Pressed() { SetDimVisible(2, true); }
void UWBP_Lobby::OnA2Released() { SetDimVisible(2, false); }

void UWBP_Lobby::OnA3Pressed() { SetDimVisible(3, true); }
void UWBP_Lobby::OnA3Released() { SetDimVisible(3, false); }

// ����
void UWBP_Lobby::OnA0Clicked() { if (!PS_Cached || PS_Cached->bReady) return; SetDimVisible(0, false); BP_SetAppearance(0); }
void UWBP_Lobby::OnA1Clicked() { if (!PS_Cached || PS_Cached->bReady) return; SetDimVisible(1, false); BP_SetAppearance(1); }
void UWBP_Lobby::OnA2Clicked() { if (!PS_Cached || PS_Cached->bReady) return; SetDimVisible(2, false); BP_SetAppearance(2); }
void UWBP_Lobby::OnA3Clicked() { if (!PS_Cached || PS_Cached->bReady) return; SetDimVisible(3, false); BP_SetAppearance(3); }

void UWBP_Lobby::OnPSChanged()
{
    UE_LOG(LogTemp, Log, TEXT("[LobbyUI] OnPSChanged: PreviewColor=%d"),
        (int32)(PS_Cached ? PS_Cached->PreviewColor : ELobbyColor::None)); // ���� �α�

    // ���� UI ���� ��ƾ ����
    UpdateAppearanceHighlight();
    UpdateReadyToggleUI();
    UpdateGameStartUI();
    RefreshOtherSlots();

    // �߾� ������ �ݿ� (���� '���� �ٲ� ���'���� ����)
    if (!Rig_Self || !PS_Cached)
    {
        return;
    }

    const int32      NewAppearanceId = PS_Cached->AppearanceId;
    const bool       bReady = PS_Cached->bReady;
    const ELobbyColor PreviewC = PS_Cached->PreviewColor;
    const ELobbyColor SelectedC = PS_Cached->SelectedColor;

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
}


void UWBP_Lobby::UpdateAppearanceHighlight()
{
    for (UBorder* B : AllSel()) if (B) B->SetVisibility(ESlateVisibility::Hidden);

    int32 Idx = (PS_Cached ? PS_Cached->AppearanceId : -1);
    if (Idx >= 0 && Idx <= 3)           
    {
        TArray<UBorder*> S = AllSel();
        if (S.IsValidIndex(Idx) && S[Idx])
            S[Idx]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }
}


void UWBP_Lobby::BindColorButtons()
{
    if (Btn_ColorR) Btn_ColorR->OnClicked.AddDynamic(this, &UWBP_Lobby::OnClick_ColorR);
    if (Btn_ColorG) Btn_ColorG->OnClicked.AddDynamic(this, &UWBP_Lobby::OnClick_ColorG);
    if (Btn_ColorB) Btn_ColorB->OnClicked.AddDynamic(this, &UWBP_Lobby::OnClick_ColorB);
}

// ��
void UWBP_Lobby::OnClick_ColorR() { if (!PS_Cached || PS_Cached->bReady) return; BP_SetPreviewColor(ELobbyColor::Red); }
void UWBP_Lobby::OnClick_ColorG() { if (!PS_Cached || PS_Cached->bReady) return; BP_SetPreviewColor(ELobbyColor::Green); }
void UWBP_Lobby::OnClick_ColorB() { if (!PS_Cached || PS_Cached->bReady) return; BP_SetPreviewColor(ELobbyColor::Blue); }

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
        if (auto PS = PC->GetPlayerState<ALobbyPlayerState>())
        {
            PS_Cached = PS;
            PS_Cached->OnAnyLobbyFieldChanged.AddDynamic(this, &UWBP_Lobby::OnPSChanged);
            bPSBound = true;

            UpdateAppearanceHighlight();

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
    if (Btn_S4) Btn_S4->OnClicked.AddDynamic(this, &UWBP_Lobby::OnClick_S4);
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

    const bool bHasColor = (PS_Cached->PreviewColor != ELobbyColor::None);
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
    if (Btn_S4)     Btn_S4->SetIsEnabled(bEnableEdit);

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
static FLinearColor ColorForSlot(const ALobbyPlayerState* PS)
{
    if (!PS) return FLinearColor(0, 0, 0, 0);
    const ELobbyColor C = PS->bReady ? PS->SelectedColor : PS->PreviewColor;
    switch (C)
    {
    case ELobbyColor::Red:   return FLinearColor(1, 0, 0, 1);
    case ELobbyColor::Green: return FLinearColor(0, 1, 0, 1);
    case ELobbyColor::Blue:  return FLinearColor(0, 0, 1, 1);
    default:                 return FLinearColor(0, 0, 0, 0);
    }
}

void UWBP_Lobby::RefreshOtherSlots()
{
    // GS/PC Ȯ��
    auto* GS = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
    auto* PC = GetOwningPlayer<ALobbyPlayerController>();   // �� ��Ʈ�ѷ��� �츮 Ÿ������
    if (!GS || !PC) return;

    // �� PS
    auto* SelfPS = PC->GetPlayerState<ALobbyPlayerState>();

    // Ÿ�� ���(�ִ� 2��)
    TArray<ALobbyPlayerState*> Others;
    Others.Reserve(2);
    for (APlayerState* Base : GS->PlayerArray)
    {
        if (auto* LPS = Cast<ALobbyPlayerState>(Base))
        {
            if (LPS != SelfPS) Others.Add(LPS);
        }
    }

    // ��/�� ������ ����(���� PlayerId�� ����)
    Others.Sort([](const ALobbyPlayerState& A, const ALobbyPlayerState& B)
        {
            return A.GetPlayerId() < B.GetPlayerId();
        });

    // ��/READY �� �ݿ�(���� ���� ����)
    auto FillMini = [](UBorder* Swatch, UTextBlock* TxtReady, const ALobbyPlayerState* PS)
        {
            if (Swatch)   Swatch->SetBrushColor(ColorForSlot(PS));
            if (TxtReady) TxtReady->SetText((PS && PS->bReady) ? FText::FromString(TEXT("READY")) : FText());
        };

    ALobbyPlayerState* L = Others.IsValidIndex(0) ? Others[0] : nullptr;
    ALobbyPlayerState* R = Others.IsValidIndex(1) ? Others[1] : nullptr;

    // ��/READY ���� ����ó��
    FillMini(Swatch_Color_OtherL, Txt_Ready_OtherL, L);
    FillMini(Swatch_Color_OtherR, Txt_Ready_OtherR, R);

    // �� Ready �� ��븸 ����� ĸó (���� RT�� 1������)
    // ����� ó��
    if (L && L->bReady) { PC->CaptureMiniFor(L, /*Left*/true); }
    else { PC->ClearMiniRT(/*Left*/true); }      // �� Unready/���� �� ����

    if (R && R->bReady) { PC->CaptureMiniFor(R, /*Left*/false); }
    else { PC->ClearMiniRT(/*Left*/false); }     // �� Unready/���� �� ����

}

void UWBP_Lobby::RebindOtherPSDelegates()
{
    auto* GS = GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
    auto* PC = GetOwningPlayer();
    if (!GS || !PC) return;

    auto* SelfPS = PC->GetPlayerState<ALobbyPlayerState>();

    // ���� ���ε� ����
    for (auto& Weak : BoundOtherPS)
        if (auto* PS = Weak.Get())
            PS->OnAnyLobbyFieldChanged.RemoveDynamic(this, &UWBP_Lobby::OnAnyPSChanged);
    BoundOtherPS.Empty();

    // �� ���ε�
    for (APlayerState* Base : GS->PlayerArray)
        if (auto* LPS = Cast<ALobbyPlayerState>(Base))
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


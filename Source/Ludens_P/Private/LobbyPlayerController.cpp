// LobbyPlayerController.cpp
#include "LobbyPlayerController.h"
#include "PlayerState_Real.h" // ���� �Ϸ�
#include "LobbyGameState.h"
#include "WBP_Lobby.h"      
#include "LobbyPreviewRig.h" 
#include "LobbyGameMode.h" 
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "GameFramework/Actor.h"


DEFINE_LOG_CATEGORY_STATIC(LogLobbyPC, Log, All);

// ===== ���� ���� =====
APlayerState_Real* ALobbyPlayerController::GetLobbyPS() const
{
    return GetPlayerState<APlayerState_Real>();
}

ALobbyGameState* ALobbyPlayerController::GetLobbyGS() const
{
    return GetWorld() ? GetWorld()->GetGameState<ALobbyGameState>() : nullptr;
}
void ALobbyPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // ���� ��Ʈ�ѷ��� �ƴϸ� �ƹ� �͵� �� �� (UI/������� ���� ȭ�� ����)
    if (!IsLocalController())
    {
        return;
    }

    bShowMouseCursor = true;

    // 1) UI ���� ����/ǥ��
    if (LobbyWidgetClass)
    {
        LobbyWidget = CreateWidget<UWBP_Lobby>(this, LobbyWidgetClass);
        if (LobbyWidget) { LobbyWidget->AddToViewport(); }
    }

    // 2) ���� RT ����(���� ����)
    auto MakeRT = [](UObject* Outer, int32 W, int32 H, const FLinearColor& Clear)
        {
            UTextureRenderTarget2D* RT = NewObject<UTextureRenderTarget2D>(Outer);
            RT->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;
            RT->ClearColor = Clear;
            RT->InitAutoFormat(W, H);
            RT->UpdateResourceImmediate(true);
            return RT;
        };
    RT_SelfDyn = MakeRT(this, 1024, 1024, FLinearColor::White);
    RT_OtherLDyn = MakeRT(this, 256, 256, FLinearColor::White);
    RT_OtherRDyn = MakeRT(this, 256, 256, FLinearColor::White);

    // 3) ������ ���� ����
    SpawnAndWirePreviewRigs();

    // 4) �߾� ������ ���׿� '���� RT' ����(���� RT�� ��� �� ��)
    if (Rig_Self) { Rig_Self->SetRenderTarget(RT_SelfDyn); }

    // 5) ������ ���� ����/RT ����
    if (LobbyWidget)
    {
        LobbyWidget->InitPreviewRefs(Rig_Self, Rig_OtherL, Rig_OtherR);
        LobbyWidget->SetPreviewRenderTargets(RT_SelfDyn, RT_OtherLDyn, RT_OtherRDyn);
    }

    // 6) UI Only + ��� �Է� ����
    {
        FInputModeUIOnly Mode;
        if (LobbyWidget) { Mode.SetWidgetToFocus(LobbyWidget->TakeWidget()); }
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(Mode);
        SetIgnoreMoveInput(true);
        SetIgnoreLookInput(true);
    }

    // ������ ���׵� ���� ����, ����Ͽ� ���׵� �ϳ� �� ����(ȭ�� ��)
    if (PreviewRigClass)
    {
        FTransform T;
        T.SetLocation(FVector(100000, 0, 0)); // ȭ�� �� �� ��
        SpawnThumbRigs();
    }

    GetWorldTimerManager().SetTimer(
        PSBindTimer, this, &ALobbyPlayerController::BindAllPSDelegates,
        0.5f, true, 0.1f);
}



// ===== ������ ���� ���� & RT �輱 =====
void ALobbyPlayerController::SpawnAndWirePreviewRigs()
{
    if (!PreviewRigClass) return;

    auto SpawnOne = [&](ALobbyPreviewRig*& OutRig, const FVector& Loc)
        {
            FActorSpawnParameters Params;
            Params.Owner = this;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            FTransform Xf; Xf.SetLocation(Loc);
            OutRig = GetWorld()->SpawnActor<ALobbyPreviewRig>(PreviewRigClass, Xf, Params);

            if (OutRig && OutRig->PreviewMesh)
            {
                // ������ ����/��Ʈ ���� ����(����)
                OutRig->PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                OutRig->PreviewMesh->SetGenerateOverlapEvents(false);
            }
        };

    // �� ���� �� ������ ���� �� �� ī�޶� �������ҿ� �� ����
    const FVector Base(100000.f, 0.f, 0.f);
    SpawnOne(Rig_Self, Base + FVector(0.f, 0.f, 0.f));
    SpawnOne(Rig_OtherL, Base + FVector(0.f, 500.f, 0.f));
    SpawnOne(Rig_OtherR, Base + FVector(0.f, -500.f, 0.f));


    // ������ ���� ���� ����(�̹� �����ø� ����)
    if (LobbyWidget)
    {
        LobbyWidget->InitPreviewRefs(Rig_Self, Rig_OtherL, Rig_OtherR);
    }
}


// ===== ���� RPC ���� =====
void ALobbyPlayerController::ServerSetAppearance_Implementation(int32 InAppearanceId)
{
    if (APlayerState_Real* PS = GetLobbyPS())
    {
        PS->AppearanceId = FMath::Clamp(InAppearanceId, 0, 3);
        PS->NotifyAnyLobbyFieldChanged();
        PS->ForceNetUpdate();

        UE_LOG(LogTemp, Display, TEXT("[RPC] Appear=%d  Preview=%d  Selected=%d  Ready=%d"),
            PS->AppearanceId, (int)PS->PreviewColor, (int)PS->SelectedColor, (int)PS->bReady);
    }


}

void ALobbyPlayerController::ServerSetSubskill_Implementation(int32 InSubskillId)
{
    if (APlayerState_Real* PS = GetLobbyPS())
    {
        PS->SubskillId = FMath::Clamp(InSubskillId, 0, 4);
        PS->NotifyAnyLobbyFieldChanged();
        PS->ForceNetUpdate();
    }
}

void ALobbyPlayerController::ServerSetPreviewColor_Implementation(ELobbyColor InColor)
{
    UE_LOG(LogTemp, Log, TEXT("[LobbyPC] ServerSetPreviewColor InColor=%d HasAuthority=%d"),  // LOG B
        (int32)InColor, (int32)HasAuthority());

    if (APlayerState_Real* PS = GetLobbyPS())
    {
        if (!PS->bReady)
        {
            PS->PreviewColor = InColor;
            PS->NotifyAnyLobbyFieldChanged(); // ȣ��Ʈ ��� ����
            PS->ForceNetUpdate();
        }

        UE_LOG(LogTemp, Display, TEXT("[RPC] Appear=%d  Preview=%d  Selected=%d  Ready=%d"),
            PS->AppearanceId, (int)PS->PreviewColor, (int)PS->SelectedColor, (int)PS->bReady);
    }

}



void ALobbyPlayerController::ServerReadyOn_Implementation(ELobbyColor Requested)
{
    APlayerState_Real* PS = GetLobbyPS();
    ALobbyGameState* GS = GetLobbyGS();
    if (!PS || !GS) return;

    if (PS->bReady) return; // �̹� Ready�� ����

    // ��ȿ�� ���� (���� ����)
    const bool bHasColor = (PS->PreviewColor != ELobbyColor::None);
    const bool bHasSkill = (PS->SubskillId >= 0 && PS->SubskillId <= 4);
    const bool bHasAppearance = (PS->AppearanceId >= 0 && PS->AppearanceId <= 3);

    UE_LOG(LogTemp, Display, TEXT("[ReadyCheck] Appear=%d SelColor=%d Skill=%d  -> bHasA=%d bHasC=%d bHasS=%d"),
        PS->AppearanceId, (int)PS->SelectedColor, PS->SubskillId,
        bHasAppearance, bHasColor, bHasSkill);

    if (!(bHasColor && bHasSkill && bHasAppearance))
    {
        return; // ������ �� ���� (S3���� ���� �佺Ʈ ����)
    }

    // ��û �� ����: None���� ���� ���� PreviewColor ���
    const ELobbyColor ColorToLock = (Requested != ELobbyColor::None) ? Requested : PS->PreviewColor;

    const int32 MyId = PS->GetPlayerId();
    const bool bLocked = GS->TryLockColor(MyId, ColorToLock);

    if (bLocked)
    {
        PS->SelectedColor = ColorToLock;
        PS->bReady = true;

        ///
        // [�ű�] �� �� = ���� �� ������ PS�� Ŀ��(�������� 1ȸ)
        auto MapToEnemyColor = [](ELobbyColor C)
            {
                switch (C) {
                case ELobbyColor::Red:   return EEnemyColor::Red;
                case ELobbyColor::Green: return EEnemyColor::Green;
                case ELobbyColor::Blue:  return EEnemyColor::Blue;
                default:                 return EEnemyColor::Red; // ����
                }
            };
        /// PS->PlayerColor = MapToEnemyColor(PS->SelectedColor);  // 
        PS->ForceNetUpdate();
        ///


        // ȣ��Ʈ(���� ����) ���� ��� ����
        PS->NotifyAnyLobbyFieldChanged();

        GS->ReadyCount++;
        GS->NotifyReadyCountChanged();   // �� ȣ��Ʈ UI Ȱ��ȭ ���� ��� �ݿ�

        PS->ForceNetUpdate();
    }
    else
    {
        // �浹: S3���� UI �佺Ʈ ó�� ����
    }

    UE_LOG(LogTemp, Display, TEXT("[RPC] Appear=%d  Preview=%d  Selected=%d  Ready=%d"),
        PS->AppearanceId, (int)PS->PreviewColor, (int)PS->SelectedColor, (int)PS->bReady);

}


void ALobbyPlayerController::ServerReadyOff_Implementation()
{
    APlayerState_Real* PS = GetLobbyPS();
    ALobbyGameState* GS = GetLobbyGS();
    if (!PS || !GS) return;

    if (PS->bReady)
    {
        GS->UnlockColor(PS->GetPlayerId(), PS->SelectedColor); // �� ��� ����
        PS->SelectedColor = ELobbyColor::None;                  // Ŀ�� �� �ʱ�ȭ
        PS->bReady = false;                                     // �غ� ����

        // UI ����ȭ
        PS->NotifyAnyLobbyFieldChanged();

        GS->ReadyCount = FMath::Max(0, GS->ReadyCount - 1);
        GS->NotifyReadyCountChanged();

        PS->ForceNetUpdate();
    }
}



void ALobbyPlayerController::CaptureMiniFor(APlayerState* OtherPS, bool bLeftSlot)
{

    if (!IsLocalController() || !OtherPS) return;

    // �� ��/�� ���� �ڱ� ���� ���
    ALobbyPreviewRig* Rig = bLeftSlot ? Rig_ThumbL : Rig_ThumbR;
    if (!Rig) return;

    auto* LPS = Cast<APlayerState_Real>(OtherPS);

    UE_LOG(LogTemp, Display, TEXT("[Mini] %s id=%d Ready=%d Prev=%d Sel=%d slot=%s"),
        *LPS->GetPlayerName(), LPS->GetPlayerId(),
        (int)LPS->bReady, (int)LPS->PreviewColor, (int)LPS->SelectedColor,
        bLeftSlot ? TEXT("L") : TEXT("R"));

    if (!LPS) return;

    // ����
    if (LPS->AppearanceId >= 0)
        Rig->SetAppearance(LPS->AppearanceId);

    // ���� + ReadyView ��Ģ(Ready ? Selected : Preview)
    if (LPS->bReady)
    {
        Rig->SetSelectedColor(LPS->SelectedColor);
        Rig->SetReadyView(true);
    }
    else
    {
        Rig->SetPreviewColor(LPS->PreviewColor);
        Rig->SetReadyView(false);
    }

    Rig->SnapYawToZero();
    Rig->RequestCapture();      // �� RT ��ü ���� �ٷ� ĸó
}





void ALobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (LobbyWidget)
    {
        LobbyWidget->RemoveFromParent();
        LobbyWidget = nullptr;
    }

    // �̴� RT ���� + ������ ����
    ClearMiniRT(/*Left*/true);
    ClearMiniRT(/*Left*/false);
    RT_SelfDyn = nullptr;
    RT_OtherLDyn = nullptr;
    RT_OtherRDyn = nullptr;

    // �� UE5�� ���� �ı�
    auto SafeDestroy = [&](ALobbyPreviewRig*& Rig)
        {
            if (IsValid(Rig)) // null�� �ƴϰ� pending kill�� �ƴ�
            {
                if (!Rig->IsActorBeingDestroyed())
                {
                    Rig->Destroy();
                }
            }
            Rig = nullptr;
        };

    SafeDestroy(Rig_Self);
    SafeDestroy(Rig_OtherL);
    SafeDestroy(Rig_OtherR);
    SafeDestroy(Rig_ThumbL);
    SafeDestroy(Rig_ThumbR);

    Super::EndPlay(EndPlayReason);
}

void ALobbyPlayerController::ClearMiniRT(bool bLeftSlot, const FLinearColor& Color)
{
    UTextureRenderTarget2D* Target = bLeftSlot ? RT_OtherLDyn : RT_OtherRDyn;
    if (Target)
    {
        UKismetRenderingLibrary::ClearRenderTarget2D(this, Target, Color);
    }
}


void ALobbyPlayerController::ServerGameStart_Implementation()
{
    // ���������� ó�� (�������� �ÿ� ����: ȣ��Ʈ PC�� ���)
    if (!HasAuthority()) return;
    if (!IsLocalController()) return;

    UE_LOG(LogTemp, Display, TEXT("ServerGameStart by Host"));

    if (ALobbyGameMode* GM = GetWorld() ? GetWorld()->GetAuthGameMode<ALobbyGameMode>() : nullptr)
    {
        GM->StartGameIfAllReady();
    }
}

void ALobbyPlayerController::BindAllPSDelegates()
{
    if (AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState<AGameStateBase>() : nullptr)
    {
        bool bNewBound = false;
        for (APlayerState* PS : GS->PlayerArray)
        {
            if (APlayerState_Real* LPS = Cast<APlayerState_Real>(PS))
            {
                if (!LPS->OnAnyLobbyFieldChanged.IsAlreadyBound(this, &ALobbyPlayerController::OnAnyPSChangedHandler))
                {
                    LPS->OnAnyLobbyFieldChanged.AddDynamic(this, &ALobbyPlayerController::OnAnyPSChangedHandler);
                    bNewBound = true;
                }
            }
        }
        if (bNewBound) RefreshMiniSlots(); // ���� ���ε��Ǹ� �� �� ��ü ���� ĸó
    }
}

void ALobbyPlayerController::RefreshMiniSlots()
{
    AGameStateBase* GS = GetWorld() ? GetWorld()->GetGameState<AGameStateBase>() : nullptr;
    if (!GS) return;

    TArray<APlayerState_Real*> Others;
    for (APlayerState* PS : GS->PlayerArray)
    {
        if (PS && PS != PlayerState)
            if (auto* LPS = Cast<APlayerState_Real>(PS)) Others.Add(LPS);
    }

    // ���� ���� ����(�÷��̾� ID ��������) �� ��/�� ���� ��ġ
    Others.Sort([](const APlayerState_Real& A, const APlayerState_Real& B)
        { return A.GetPlayerId() < B.GetPlayerId(); });

    ClearMiniRT(true);
    ClearMiniRT(false);

    if (Others.Num() > 0) CaptureMiniFor(Others[0], /*Left*/true);
    if (Others.Num() > 1)
    {
        TWeakObjectPtr<APlayerState> PS1 = Others[1];
        GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this, PS1]()
            {
                if (PS1.IsValid()) CaptureMiniFor(PS1.Get(), /*Left*/false);
            }));
    }

}


void ALobbyPlayerController::OnAnyPSChangedHandler()
{
    RefreshMiniSlots(); // ���� �ٲ�� ��/�� ������ �ٽ� ��´�
}

void ALobbyPlayerController::SpawnThumbRigs()
{
    if (!PreviewRigClass) return;

    auto SpawnOne = [&](ALobbyPreviewRig*& OutRig, const FVector& Loc)
        {
            FActorSpawnParameters Params;
            Params.Owner = this;
            Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            FTransform Xf; Xf.SetLocation(Loc);
            OutRig = GetWorld()->SpawnActor<ALobbyPreviewRig>(PreviewRigClass, Xf, Params);
            if (OutRig && OutRig->PreviewMesh)
            {
                OutRig->PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                OutRig->PreviewMesh->SetGenerateOverlapEvents(false);
            }
        };

    const FVector Base(100000.f, 0.f, 0.f); // ȭ�� ��
    SpawnOne(Rig_ThumbL, Base + FVector(0.f, 800.f, 0.f));
    SpawnOne(Rig_ThumbR, Base + FVector(0.f, -800.f, 0.f));

    // ���� �ڱ� RT�� "����" ����
    if (Rig_ThumbL && RT_OtherLDyn) Rig_ThumbL->SetRenderTarget(RT_OtherLDyn);
    if (Rig_ThumbR && RT_OtherRDyn) Rig_ThumbR->SetRenderTarget(RT_OtherRDyn);
}

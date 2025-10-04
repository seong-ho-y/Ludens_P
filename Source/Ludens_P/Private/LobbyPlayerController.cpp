// LobbyPlayerController.cpp
#include "LobbyPlayerController.h"
#include "PlayerState_Real.h" // 수정 완료
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

// ===== 내부 헬퍼 =====
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

    // 로컬 컨트롤러가 아니면 아무 것도 안 함 (UI/프리뷰는 각자 화면 전용)
    if (!IsLocalController())
    {
        return;
    }

    bShowMouseCursor = true;

    // 1) UI 먼저 생성/표시
    if (LobbyWidgetClass)
    {
        LobbyWidget = CreateWidget<UWBP_Lobby>(this, LobbyWidgetClass);
        if (LobbyWidget) { LobbyWidget->AddToViewport(); }
    }

    // 2) 동적 RT 생성(각자 전용)
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

    // 3) 프리뷰 리그 스폰
    SpawnAndWirePreviewRigs();

    // 4) 중앙 프리뷰 리그에 '동적 RT' 연결(에셋 RT는 사용 안 함)
    if (Rig_Self) { Rig_Self->SetRenderTarget(RT_SelfDyn); }

    // 5) 위젯에 리그 참조/RT 전달
    if (LobbyWidget)
    {
        LobbyWidget->InitPreviewRefs(Rig_Self, Rig_OtherL, Rig_OtherR);
        LobbyWidget->SetPreviewRenderTargets(RT_SelfDyn, RT_OtherLDyn, RT_OtherRDyn);
    }

    // 6) UI Only + 배경 입력 차단
    {
        FInputModeUIOnly Mode;
        if (LobbyWidget) { Mode.SetWidgetToFocus(LobbyWidget->TakeWidget()); }
        Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        SetInputMode(Mode);
        SetIgnoreMoveInput(true);
        SetIgnoreLookInput(true);
    }

    // 프리뷰 리그들 스폰 이후, 썸네일용 리그도 하나 더 스폰(화면 밖)
    if (PreviewRigClass)
    {
        FTransform T;
        T.SetLocation(FVector(100000, 0, 0)); // 화면 밖 먼 곳
        SpawnThumbRigs();
    }

    GetWorldTimerManager().SetTimer(
        PSBindTimer, this, &ALobbyPlayerController::BindAllPSDelegates,
        0.5f, true, 0.1f);
}



// ===== 프리뷰 리그 스폰 & RT 배선 =====
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
                // 배경과의 물리/히트 간섭 제거(안전)
                OutRig->PreviewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                OutRig->PreviewMesh->SetGenerateOverlapEvents(false);
            }
        };

    // ★ 아주 먼 곳으로 스폰 → 주 카메라 프러스텀에 안 잡힘
    const FVector Base(100000.f, 0.f, 0.f);
    SpawnOne(Rig_Self, Base + FVector(0.f, 0.f, 0.f));
    SpawnOne(Rig_OtherL, Base + FVector(0.f, 500.f, 0.f));
    SpawnOne(Rig_OtherR, Base + FVector(0.f, -500.f, 0.f));


    // 위젯에 리그 참조 전달(이미 있으시면 유지)
    if (LobbyWidget)
    {
        LobbyWidget->InitPreviewRefs(Rig_Self, Rig_OtherL, Rig_OtherR);
    }
}


// ===== 서버 RPC 구현 =====
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
            PS->NotifyAnyLobbyFieldChanged(); // 호스트 즉시 갱신
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

    if (PS->bReady) return; // 이미 Ready면 무시

    // 유효성 검증 (서버 권위)
    const bool bHasColor = (PS->PreviewColor != ELobbyColor::None);
    const bool bHasSkill = (PS->SubskillId >= 0 && PS->SubskillId <= 4);
    const bool bHasAppearance = (PS->AppearanceId >= 0 && PS->AppearanceId <= 3);

    UE_LOG(LogTemp, Display, TEXT("[ReadyCheck] Appear=%d SelColor=%d Skill=%d  -> bHasA=%d bHasC=%d bHasS=%d"),
        PS->AppearanceId, (int)PS->SelectedColor, PS->SubskillId,
        bHasAppearance, bHasColor, bHasSkill);

    if (!(bHasColor && bHasSkill && bHasAppearance))
    {
        return; // 미충족 → 무시 (S3에서 실패 토스트 예정)
    }

    // 요청 색 보정: None으로 오면 현재 PreviewColor 사용
    const ELobbyColor ColorToLock = (Requested != ELobbyColor::None) ? Requested : PS->PreviewColor;

    const int32 MyId = PS->GetPlayerId();
    const bool bLocked = GS->TryLockColor(MyId, ColorToLock);

    if (bLocked)
    {
        PS->SelectedColor = ColorToLock;
        PS->bReady = true;

        ///
        // [신규] 상성 색 = 선택 색 매핑을 PS에 커밋(서버에서 1회)
        auto MapToEnemyColor = [](ELobbyColor C)
            {
                switch (C) {
                case ELobbyColor::Red:   return EEnemyColor::Red;
                case ELobbyColor::Green: return EEnemyColor::Green;
                case ELobbyColor::Blue:  return EEnemyColor::Blue;
                default:                 return EEnemyColor::Red; // 폴백
                }
            };
        /// PS->PlayerColor = MapToEnemyColor(PS->SelectedColor);  // 
        PS->ForceNetUpdate();
        ///


        // 호스트(서버 로컬) 위젯 즉시 갱신
        PS->NotifyAnyLobbyFieldChanged();

        GS->ReadyCount++;
        GS->NotifyReadyCountChanged();   // ★ 호스트 UI 활성화 조건 즉시 반영

        PS->ForceNetUpdate();
    }
    else
    {
        // 충돌: S3에서 UI 토스트 처리 예정
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
        GS->UnlockColor(PS->GetPlayerId(), PS->SelectedColor); // 색 잠금 해제
        PS->SelectedColor = ELobbyColor::None;                  // 커밋 색 초기화
        PS->bReady = false;                                     // 준비 해제

        // UI 동기화
        PS->NotifyAnyLobbyFieldChanged();

        GS->ReadyCount = FMath::Max(0, GS->ReadyCount - 1);
        GS->NotifyReadyCountChanged();

        PS->ForceNetUpdate();
    }
}



void ALobbyPlayerController::CaptureMiniFor(APlayerState* OtherPS, bool bLeftSlot)
{

    if (!IsLocalController() || !OtherPS) return;

    // ★ 좌/우 각각 자기 리그 사용
    ALobbyPreviewRig* Rig = bLeftSlot ? Rig_ThumbL : Rig_ThumbR;
    if (!Rig) return;

    auto* LPS = Cast<APlayerState_Real>(OtherPS);

    UE_LOG(LogTemp, Display, TEXT("[Mini] %s id=%d Ready=%d Prev=%d Sel=%d slot=%s"),
        *LPS->GetPlayerName(), LPS->GetPlayerId(),
        (int)LPS->bReady, (int)LPS->PreviewColor, (int)LPS->SelectedColor,
        bLeftSlot ? TEXT("L") : TEXT("R"));

    if (!LPS) return;

    // 외형
    if (LPS->AppearanceId >= 0)
        Rig->SetAppearance(LPS->AppearanceId);

    // 색상 + ReadyView 규칙(Ready ? Selected : Preview)
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
    Rig->RequestCapture();      // ← RT 교체 없이 바로 캡처
}





void ALobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (LobbyWidget)
    {
        LobbyWidget->RemoveFromParent();
        LobbyWidget = nullptr;
    }

    // 미니 RT 리셋 + 포인터 정리
    ClearMiniRT(/*Left*/true);
    ClearMiniRT(/*Left*/false);
    RT_SelfDyn = nullptr;
    RT_OtherLDyn = nullptr;
    RT_OtherRDyn = nullptr;

    // ★ UE5식 안전 파괴
    auto SafeDestroy = [&](ALobbyPreviewRig*& Rig)
        {
            if (IsValid(Rig)) // null도 아니고 pending kill도 아님
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
    // 서버에서만 처리 (리슨서버 시연 가정: 호스트 PC만 통과)
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
        if (bNewBound) RefreshMiniSlots(); // 새로 바인딩되면 한 번 전체 새로 캡처
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

    // 정렬 기준 고정(플레이어 ID 오름차순) → 좌/우 안정 배치
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
    RefreshMiniSlots(); // 누가 바뀌든 좌/우 슬롯을 다시 찍는다
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

    const FVector Base(100000.f, 0.f, 0.f); // 화면 밖
    SpawnOne(Rig_ThumbL, Base + FVector(0.f, 800.f, 0.f));
    SpawnOne(Rig_ThumbR, Base + FVector(0.f, -800.f, 0.f));

    // 각자 자기 RT에 "고정" 연결
    if (Rig_ThumbL && RT_OtherLDyn) Rig_ThumbL->SetRenderTarget(RT_OtherLDyn);
    if (Rig_ThumbR && RT_OtherRDyn) Rig_ThumbR->SetRenderTarget(RT_OtherRDyn);
}

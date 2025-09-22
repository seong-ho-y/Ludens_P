#include "LobbyPreviewRig.h"
#include "LudensAppearanceData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

ALobbyPreviewRig::ALobbyPreviewRig()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = false;
    SetReplicateMovement(false);

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    PreviewMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PreviewMesh"));
    PreviewMesh->SetupAttachment(Root);
    PreviewMesh->SetMobility(EComponentMobility::Movable);
    PreviewMesh->SetCastShadow(false);
    PreviewMesh->bCastDynamicShadow = false;

    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
    SceneCapture->SetupAttachment(Root);

    // (선택) 보기 좋게 기본 트랜스폼
    SceneCapture->SetRelativeLocation(FVector(140.f, 0.f, 80.f));
    SceneCapture->SetRelativeRotation(FRotator(-10.f, 180.f, 0.f));
    SceneCapture->FOVAngle = 30.f;
}

void ALobbyPreviewRig::BeginPlay()
{
    Super::BeginPlay();

    if (PreviewMesh)
    {
        BaseYaw = PreviewMesh->GetRelativeRotation().Yaw;
        AccumYaw = 0.f;
        ApplyYaw();
    }

    if (SceneCapture)
    {
        SceneCapture->bCaptureEveryFrame = false;
        SceneCapture->bCaptureOnMovement = false;
        SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
        if (SceneCapture->TextureTarget)
        {
            SceneCapture->TextureTarget->ClearColor = FLinearColor::White;
        }
    }

    IsolateToOwnMesh();

    // 스폰 직후 1회 적용 (A0/미선택 → 코드 내부에서 Red 폴백)
    if (CurrentAppearanceId < 0) CurrentAppearanceId = 0;
    ApplyPreviewFromDB();
}

void ALobbyPreviewRig::IsolateToOwnMesh()
{
    if (!SceneCapture || !PreviewMesh) return;
    SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    SceneCapture->ShowOnlyActors.Empty();
    SceneCapture->ShowOnlyComponents.Empty();
    SceneCapture->ShowOnlyComponent(PreviewMesh);
}

void ALobbyPreviewRig::RequestCapture()
{
    if (SceneCapture) SceneCapture->CaptureScene();
}

void ALobbyPreviewRig::ApplyYaw()
{
    if (PreviewMesh)
    {
        PreviewMesh->SetRelativeRotation(FRotator(0.f, BaseYaw + AccumYaw, 0.f));
    }
}

void ALobbyPreviewRig::AddYaw(float DeltaYaw)
{
    if (!PreviewMesh) return;
    AccumYaw = FMath::UnwindDegrees(AccumYaw + DeltaYaw);
    ApplyYaw();
    RequestCapture();
}

void ALobbyPreviewRig::SnapYawToZero()
{
    AccumYaw = 0.f;
    ApplyYaw();
    RequestCapture();
}

void ALobbyPreviewRig::SetRenderTarget(UTextureRenderTarget2D* Target)
{
    if (SceneCapture && Target)
    {
        SceneCapture->TextureTarget = Target;
        RequestCapture();
    }
}

void ALobbyPreviewRig::SetAppearance(int32 InAp)
{
    CurrentAppearanceId = InAp;
    ApplyPreviewFromDB();
}

void ALobbyPreviewRig::SetPreviewColor(ELobbyColor InColor)
{
    CurrentPreviewColor = InColor;
    if (!bReadyView) ApplyPreviewFromDB();
}

void ALobbyPreviewRig::SetSelectedColor(ELobbyColor InColor)
{
    CurrentSelectedColor = InColor;
    if (bReadyView) ApplyPreviewFromDB();
}

void ALobbyPreviewRig::SetReadyView(bool bOn)
{
    bReadyView = bOn;
    ApplyPreviewFromDB();
}

void ALobbyPreviewRig::ApplyPreviewFromDB()
{
    if (!AppearanceDB || !PreviewMesh) return;

    const ELobbyColor ColorToShow = bReadyView ? CurrentSelectedColor : CurrentPreviewColor;
    const int32 Ap = FMath::Max(0, CurrentAppearanceId);

    AppearanceDB->ApplyTo(PreviewMesh, Ap, ColorToShow);   
    RequestCapture();
}

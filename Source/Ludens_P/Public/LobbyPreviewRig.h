#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LobbyTypes.h"
#include "LobbyPreviewRig.generated.h"

class USceneComponent;
class USkeletalMeshComponent;
class USceneCaptureComponent2D;
class UTextureRenderTarget2D;
class ULudensAppearanceData;

UCLASS()
class LUDENS_P_API ALobbyPreviewRig : public AActor
{
    GENERATED_BODY()
public:
    ALobbyPreviewRig();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneComponent* Root;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USkeletalMeshComponent* PreviewMesh;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) USceneCaptureComponent2D* SceneCapture;

    // 외부 API
    UFUNCTION(BlueprintCallable, Category = "Lobby|Preview") void SetRenderTarget(UTextureRenderTarget2D* Target);
    UFUNCTION(BlueprintCallable, Category = "Lobby|Preview") void SetAppearance(int32 Id);
    UFUNCTION(BlueprintCallable, Category = "Lobby|Preview") void SetPreviewColor(ELobbyColor C);
    UFUNCTION(BlueprintCallable, Category = "Lobby|Preview") void SetSelectedColor(ELobbyColor C);
    UFUNCTION(BlueprintCallable, Category = "Lobby|Preview") void SetReadyView(bool bOn);
    UFUNCTION(BlueprintCallable, Category = "Lobby|Preview") void AddYaw(float DeltaYaw);
    UFUNCTION(BlueprintCallable, Category = "Lobby|Preview") void SnapYawToZero();
    UFUNCTION(BlueprintCallable, Category = "Lobby|Preview") void RequestCapture();
    UFUNCTION(BlueprintCallable, Category = "Lobby|Preview") void IsolateToOwnMesh();

    // DA 한 곳만 남김
    UPROPERTY(EditDefaultsOnly, Category = "Cosmetics|DB")
    ULudensAppearanceData* AppearanceDB = nullptr;

protected:
    virtual void BeginPlay() override;

private:
    // 회전 상태
    float BaseYaw = 0.f;
    float AccumYaw = 0.f;
    void  ApplyYaw();

    // 현재 상태
    int32       CurrentAppearanceId = -1;
    ELobbyColor CurrentPreviewColor = ELobbyColor::None;
    ELobbyColor CurrentSelectedColor = ELobbyColor::None;
    bool        bReadyView = false;

    // DA 적용 전용
    void ApplyPreviewFromDB();
};

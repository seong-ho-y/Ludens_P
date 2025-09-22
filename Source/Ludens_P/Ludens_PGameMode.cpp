// Ludens_PGameMode.cpp
#include "Ludens_PGameMode.h"
#include "Ludens_PCharacter.h"
<<<<<<< Updated upstream
=======
#include "Ludens_PPlayerController.h"     // 인게임 PC
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
>>>>>>> Stashed changes
#include "UObject/ConstructorHelpers.h"

ALudens_PGameMode::ALudens_PGameMode()
    : Super()
{
<<<<<<< Updated upstream
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

=======
    PlayerStateClass = ALobbyPlayerState::StaticClass(); // 지금은 이대로 유지
    bUseSeamlessTravel = true;

    static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(
        TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
    DefaultPawnClass = PlayerPawnClassFinder.Class;
}

// ★ 심리스 전이 시 PC 교체 보장
void ALudens_PGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
    Super::HandleSeamlessTravelPlayer(C);

    APlayerController* OldPC = Cast<APlayerController>(C);
    if (!OldPC) return;

    // 이미 인게임 PC면 교체 불필요
    if (OldPC->IsA(ALudens_PPlayerController::StaticClass()))
    {
        UE_LOG(LogTemp, Log, TEXT("[Seamless] keep PC=%s (%s)"), *OldPC->GetName(), *OldPC->GetClass()->GetName());
        return;
    }

    // 새 인게임 PC 스폰
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    Params.Owner = this;

    const FVector  SpawnLoc = (OldPC->GetPawn() ? OldPC->GetPawn()->GetActorLocation()
        : FVector::ZeroVector);
    const FRotator SpawnRot = OldPC->GetControlRotation();

    APlayerController* NewPC = GetWorld()->SpawnActor<APlayerController>(
        ALudens_PPlayerController::StaticClass(), SpawnLoc, SpawnRot, Params);

    if (!NewPC)
    {
        UE_LOG(LogTemp, Error, TEXT("[Seamless] FAILED to spawn InGame PC. keeping old PC."));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[Seamless] swap PC: %s(%s) -> %s(%s)"),
        *OldPC->GetName(), *OldPC->GetClass()->GetName(),
        *NewPC->GetName(), *NewPC->GetClass()->GetName());

    // 네트워크 연결/PlayerState 이관
    SwapPlayerControllers(OldPC, NewPC);
    C = NewPC;

    if (NewPC && NewPC->PlayerState)
    {
        if (auto* LPS = Cast<ALobbyPlayerState>(NewPC->PlayerState))
        {
            UE_LOG(LogTemp, Display, TEXT("[PostSwap] %s Appear=%d Sel=%d Prev=%d Skill=%d Ready=%d"),
                *GetNameSafe(LPS),
                LPS->AppearanceId, (int)LPS->SelectedColor, (int)LPS->PreviewColor,
                LPS->SubskillId, LPS->bReady);
        }
    }


    // 구 PC 정리
    OldPC->Destroy();
}

// ★ PlayerId ↔ PlayerStart 태그 매칭 (P0/P1/P2 …)
AActor* ALudens_PGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    if (!Player) return Super::ChoosePlayerStart(Player);

    int32 Id = 0;
    if (const APlayerState* PS = Player->PlayerState)
        Id = PS->GetPlayerId();

    const FName WantedTag(*FString::Printf(TEXT("P%d"), Id));

    APlayerStart* Fallback = nullptr;
    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        APlayerStart* Start = *It;
        if (!IsValid(Start)) continue;

        if (!Fallback) Fallback = Start;
        if (Start->ActorHasTag(WantedTag))
        {
            UE_LOG(LogTemp, Display, TEXT("[ChooseStart] Id=%d -> %s"), Id, *WantedTag.ToString());
            return Start;
        }
    }
    UE_LOG(LogTemp, Display, TEXT("[ChooseStart] Id=%d -> fallback"), Id);
    return Fallback ? Fallback : Super::ChoosePlayerStart(Player);
>>>>>>> Stashed changes
}

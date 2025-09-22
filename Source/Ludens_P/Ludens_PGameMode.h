// Ludens_PGameMode.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
<<<<<<< Updated upstream
=======
#include "LobbyPlayerState.h"           // 지금은 이 PS를 유지
>>>>>>> Stashed changes
#include "Ludens_PGameMode.generated.h"

UCLASS(minimalapi)
class ALudens_PGameMode : public AGameModeBase
{
<<<<<<< Updated upstream
	GENERATED_BODY()

public:
	ALudens_PGameMode();
};



=======
    GENERATED_BODY()
public:
    ALudens_PGameMode();

    // ★ 심리스 교체 훅
    virtual void HandleSeamlessTravelPlayer(AController*& C) override;

    // ★ PlayerId ↔ PlayerStart 태그 매칭(아래 변경 2에서 구현)
    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
>>>>>>> Stashed changes

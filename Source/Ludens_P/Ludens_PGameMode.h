// Ludens_PGameMode.h
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
<<<<<<< Updated upstream
=======
#include "LobbyPlayerState.h"           // ������ �� PS�� ����
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

    // �� �ɸ��� ��ü ��
    virtual void HandleSeamlessTravelPlayer(AController*& C) override;

    // �� PlayerId �� PlayerStart �±� ��Ī(�Ʒ� ���� 2���� ����)
    virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
>>>>>>> Stashed changes

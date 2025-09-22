// Copyright Epic Games, Inc. All Rights Reserved.


#include "Ludens_PPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

void ALudens_PPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
<<<<<<< Updated upstream
=======

	// �� �Է� ���/Ŀ��
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	// �� Enhanced Input ����(���� DefaultMappingContext�� �ִٸ�)
	if (ULocalPlayer* LP = GetLocalPlayer())
		if (auto* Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
			if (InputMappingContext) Sub->AddMappingContext(InputMappingContext, 0);

	// �� Pawn ���� Ȯ��(����� �α�)
	UE_LOG(LogTemp, Display, TEXT("[FPSPC] Possessed Pawn = %s"), *GetNameSafe(GetPawn()));

	// ���� �ڵ� ����: Enhanced Input ���� ���ؽ�Ʈ �߰�
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0); // �� �� ������Ʈ ������
	}

>>>>>>> Stashed changes
}
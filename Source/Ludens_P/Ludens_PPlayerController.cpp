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

	// ① 입력 모드/커서
	bShowMouseCursor = false;
	SetInputMode(FInputModeGameOnly());

	// ② Enhanced Input 매핑(기존 DefaultMappingContext가 있다면)
	if (ULocalPlayer* LP = GetLocalPlayer())
		if (auto* Sub = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
			if (InputMappingContext) Sub->AddMappingContext(InputMappingContext, 0);

	// ③ Pawn 소유 확인(디버깅 로그)
	UE_LOG(LogTemp, Display, TEXT("[FPSPC] Possessed Pawn = %s"), *GetNameSafe(GetPawn()));

	// 기존 코드 유지: Enhanced Input 매핑 컨텍스트 추가
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputMappingContext, 0); // ← 네 프로젝트 변수명
	}

>>>>>>> Stashed changes
}
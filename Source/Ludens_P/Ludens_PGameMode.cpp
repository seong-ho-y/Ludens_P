// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ludens_PGameMode.h"

#include "EnemyAIController.h"
#include "EnemyBase.h"
#include "Ludens_PCharacter.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

ALudens_PGameMode::ALudens_PGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
void ALudens_PGameMode::BeginPlay()
{
	Super::BeginPlay();

	SpawnEnemyAtLocation(FVector(1800, 1870, 88));
}
void ALudens_PGameMode::SpawnEnemyAtLocation(FVector Location)
{
	if (HasAuthority() && EnemyToSpawn)
	{
		GetWorld()->SpawnActor<AEnemyBase>(EnemyToSpawn, Location, FRotator::ZeroRotator);
	}
}

AActor* ALudens_PGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> AllStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), AllStarts);

	for (AActor* Start : AllStarts)
	{
		if (!UsedStartSpots.Contains(Start))
		{
			UsedStartSpots.Add(Start);
			UE_LOG(LogTemp, Warning, TEXT("Player assigned to Start: %s"), *Start->GetName());
			return Start;
		}
	}

	// 모두 사용됐으면 기본 처리
	return Super::ChoosePlayerStart_Implementation(Player);
}

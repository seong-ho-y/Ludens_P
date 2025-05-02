#include "EnemyBase.h"

#include "EnemyAIController.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Combat = CreateDefaultSubobject<UCreatureCombatComponent>(TEXT("Combat"));
	WalkerAI = CreateDefaultSubobject<UWalkerAIComponent>(TEXT("WalkerAI"));

	bReplicates = true;
	SetReplicateMovement(true);

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
}
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AEnemyBase::SetActive(bool bNewActive)
{
	bActive = bNewActive;
	SetActorHiddenInGame(!bNewActive);
	SetActorEnableCollision(bNewActive);
	SetActorTickEnabled(bNewActive);

	if (Combat) Combat->SetComponentTickEnabled(bNewActive);
	if (WalkerAI) WalkerAI->SetComponentTickEnabled(bNewActive);
}

bool AEnemyBase::IsActive() const
{
	return bActive;
}

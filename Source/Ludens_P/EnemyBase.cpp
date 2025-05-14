#include "EnemyBase.h"

#include "EnemyAIController.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Combat = CreateDefaultSubobject<UCreatureCombatComponent>(TEXT("CombatComponent"));
	
	
	bReplicates = true;
	SetReplicateMovement(true);

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (!Combat)
	{
		UE_LOG(LogTemp, Error, TEXT("❌ Combat is not assigned in %s!"), *GetName());
	}
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AEnemyBase::SetActive(bool bNewActive)
{
	
	if (!Combat)
	{
		UE_LOG(LogTemp, Error, TEXT("Combat is nullptr in SetActive"));
		ensureAlwaysMsgf(false, TEXT("EnemyBase must have CombatComponent initialized!"));
		return;
	}
	bActive = bNewActive;

	SetActorHiddenInGame(!bNewActive);
	SetActorEnableCollision(bNewActive);
	SetActorTickEnabled(bNewActive);
	Combat->SetComponentTickEnabled(bNewActive);
	

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetVisibility(bNewActive, true);
		MeshComp->SetHiddenInGame(!bNewActive, true);
		MeshComp->bPauseAnims = !bNewActive;
		MeshComp->SetComponentTickEnabled(bNewActive);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ GetMesh() returned null!"));
	}
}
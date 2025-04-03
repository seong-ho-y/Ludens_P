#include "EnemyBase.h"

#include "EnemyAIController.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
	

	bReplicates = true;
	SetReplicateMovement(true);
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCharacterMovement()->MaxWalkSpeed = 200.f; // 원하는 속도로 설정 (cm/s)

}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
    
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

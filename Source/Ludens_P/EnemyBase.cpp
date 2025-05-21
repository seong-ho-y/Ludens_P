#include "EnemyBase.h"

#include "EnemyAIController.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
	NetDormancy = DORM_Never;
	Combat = CreateDefaultSubobject<UCreatureCombatComponent>(TEXT("CombatComponent"));
	
	bAlwaysRelevant = true;
	bReplicates = true;
	SetReplicatingMovement(true);

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	NetUpdateFrequency = 100.0f; // 기본은 0.1~2.0
	MinNetUpdateFrequency = 50.0f;
	/*UE_LOG(LogTemp, Warning, TEXT("🐛 EnemyBase spawned on: %s | NetRole: %d"), 
		HasAuthority() ? TEXT("Server") : TEXT("Client"),
		static_cast<int32>(GetLocalRole()));*/

	/*UE_LOG(LogTemp, Warning, TEXT("🔍 [%s] Replicated: %s | Role: %d | RemoteRole: %d"),
		*GetName(),
		bReplicates ? TEXT("true") : TEXT("false"),
		(int32)GetLocalRole(),
		(int32)GetRemoteRole());*/
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
	//UE_LOG(LogTemp, Warning, TEXT("🔧 SetActive called on %s → new state: %s"),
	//*GetName(), bNewActive ? TEXT("ACTIVE") : TEXT("INACTIVE"));

	if (!Combat)
	{
		//UE_LOG(LogTemp, Error, TEXT("Combat is nullptr in SetActive"));
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
void AEnemyBase::PostNetInit()
{
	Super::PostNetInit();

	// 서버가 지정한 대기 후, 클라이언트에서도 안전하게 SetActive 가능
	if (!HasAuthority())
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyBase::OnPostReplicationInit, 0.1f, false);
	}
}

void AEnemyBase::OnPostReplicationInit()
{
	SetActive(false); // 안전하게 비활성화
}
bool AEnemyBase::IsActive() const
{
	return bActive;
}
void AEnemyBase::MulticastSetActive_Implementation(bool bNewActive)
{
	SetActive(bNewActive);
}
	
#include "EnemyBase.h"

#include "EnemyAIController.h"
#include "ShooterAIComponent.h"
#include "StealthComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_WeaponComponent.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
	NetDormancy = DORM_Never;
	
	MaterialSlotIndex = 0; // 첫 번째 Material 슬롯에 적용
	
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
	bActive = bNewActive;

	SetActorHiddenInGame(!bNewActive);
	SetActorEnableCollision(bNewActive);
	SetActorTickEnabled(bNewActive);
	StealthComponent = FindComponentByClass<UStealthComponent>();

	if (Combat) //전투 컴포넌트 비활성화
	{
		Combat->SetComponentTickEnabled(bNewActive);
	}
	else
	{
		ensureAlwaysMsgf(false, TEXT("EnemyBase must have CombatComponent initialized!"));
	}

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
	TArray<UActorComponent*> AIComponents;
	GetComponents(UActorComponent::StaticClass(), AIComponents);
	for (UActorComponent* Component : AIComponents)
	{
		// 원하는 타입만 처리 (WalkerAI, ShooterAI 등)
		if (Component->IsA<UWalkerAIComponent>() || Component->IsA<UShooterAIComponent>())
		{
			Component->SetComponentTickEnabled(bNewActive);
		}
	}

	// 🔽 여기서 스텔스 상태 초기화
	if (StealthComponent && bActive)
	{
		StealthComponent->ResetStealthState();
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("No StealthComponent"));
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

void AEnemyBase::SetEnemyMeshMaterial(EEnemyColor NewColor)
{
	// 메시 컴포넌트가 유효한지 확인
	if (!GetMesh())
	{
		UE_LOG(LogTemp, Warning, TEXT("AEnemyBase::SetEnemyMeshMaterial - Mesh component is null!"));
		return;
	}

	// EnemyMaterials 맵에서 NewColor에 해당하는 Material을 찾습니다.
	UMaterialInterface** FoundMaterial = EnemyMaterials.Find(NewColor);

	if (FoundMaterial && *FoundMaterial)
	{
		// 찾은 Material을 메시 컴포넌트의 지정된 슬롯에 적용합니다.
		GetMesh()->SetMaterial(MaterialSlotIndex, *FoundMaterial);
		// CurrentEnemyColor = NewColor; // 필요하다면 현재 색상도 업데이트
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AEnemyBase::SetEnemyMeshMaterial - No material found for color: %d"), (uint8)NewColor);
		// 해당하는 Material이 없을 경우의 처리 (예: 기본 Material로 폴백)
	}
}
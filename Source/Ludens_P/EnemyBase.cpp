#include "EnemyBase.h"

#include "EnemyAIController.h"
#include "ShooterAIComponent.h"
#include "StealthComponent.h"
#include "TP_WeaponComponent.h"
#include "Net/UnrealNetwork.h"

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

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyBase, EnemyColor);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	NetUpdateFrequency = 100.0f; 
	MinNetUpdateFrequency = 50.0f;

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

	if (Combat) 
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
		if (Component->IsA<UWalkerAIComponent>() || Component->IsA<UShooterAIComponent>())
		{
			Component->SetComponentTickEnabled(bNewActive);
		}
	}

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

	// 클라이언트에서만 실행 (AutonomousProxy: 플레이어가 직접 조종, SimulatedProxy: 다른 플레이어에 의해 시뮬레이션)
	if (GetLocalRole() == ROLE_AutonomousProxy || GetLocalRole() == ROLE_SimulatedProxy)
	{
		UE_LOG(LogTemp, Warning, TEXT("CLIENT: PostNetInit Called! Current Color is %d"), static_cast<int32>(EnemyColor));
		
		// 네트워크 초기화가 완료되었으므로, 현재 EnemyColor 값으로 머티리얼을 확실하게 업데이트합니다.
		// 자원이 로드될 시간을 벌기 위해 약간의 딜레이를 줍니다.
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyBase::UpdateMaterial, 0.1f, false);
	}
}

void AEnemyBase::OnPostReplicationInit()
{
	SetActive(false); 
}
bool AEnemyBase::IsActive() const
{
	return bActive;
}
void AEnemyBase::MulticastSetActive_Implementation(bool bNewActive)
{
	SetActive(bNewActive);
}

void AEnemyBase::SetupEnemyForColor(EEnemyColor NewColor)
{
	if (HasAuthority())
	{
		// 이전 색상과 새 색상을 로그로 출력하여 실제로 값이 변경되는지 확인합니다.
		UE_LOG(LogTemp, Warning, TEXT("SERVER: SetupEnemyForColor Called. OldColor: %d, NewColor: %d"), static_cast<int32>(EnemyColor), static_cast<int32>(NewColor));

		if (EnemyColor != NewColor)
		{
			EnemyColor = NewColor;
			InitializeShields();
			UpdateMaterial(); // 서버에서도 즉시 머티리얼이 보이도록 직접 호출
		}
	}
}

void AEnemyBase::OnRep_EnemyColor()
{
	// OnRep 함수가 클라이언트에서 호출되었는지, 어떤 색상 값으로 호출되었는지 확인합니다.
	UE_LOG(LogTemp, Warning, TEXT("CLIENT: OnRep_EnemyColor Called! NewColor is %d"), static_cast<int32>(EnemyColor));
	
	// 자원이 로드될 시간을 벌기 위해 약간의 딜레이 후 머티리얼을 업데이트합니다.
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AEnemyBase::UpdateMaterial, 0.1f, false);
}

void AEnemyBase::UpdateMaterial()
{
	if (EnemyMaterials.Contains(EnemyColor))
	{
		UMaterialInstance* FoundMaterial = EnemyMaterials[EnemyColor];
		if (FoundMaterial && GetMesh())
		{
			GetMesh()->SetMaterial(MaterialSlotIndex, FoundMaterial);
		}
	}
}

void AEnemyBase::InitializeShields()
{
	// 기존 쉴드 정보를 모두 지웁니다.
	Shields.Empty();

	// DefaultShields 맵에 정의된 각 기본 색상에 대해 반복합니다.
	for (const TPair<EEnemyColor, int32>& ShieldInfo : DefaultShields)
	{
		const EEnemyColor& Color = ShieldInfo.Key;
		const int32& MaxShieldAmount = ShieldInfo.Value;

		// 이 적의 EnemyColor가 해당 기본 색상을 포함하는지 비트 연산으로 확인합니다.
		if (EnumHasAllFlags(EnemyColor, Color))
		{
			// 포함한다면, 현재 쉴드 맵(Shields)에 최대치로 추가합니다.
			Shields.Add(Color, MaxShieldAmount);
		}
	}
}

void AEnemyBase::ApplyShieldDamage(EEnemyColor DamageColor, int32 DamageAmount)
{
	// 데미지 타입에 해당하는 쉴드가 있는지 확인합니다.
	if (int32* CurrentShield = Shields.Find(DamageColor))
	{
		// 쉴드가 있다면 데미지를 적용합니다.
		*CurrentShield -= DamageAmount;

		// 쉴드가 0 이하로 떨어졌다면 맵에서 제거합니다.
		if (*CurrentShield <= 0)
		{
			Shields.Remove(DamageColor);
			// 여기에 쉴드가 파괴되었을 때의 로직을 추가할 수 있습니다 (예: 이펙트 재생)
		}
	}
	else
	{
		// 해당 색상의 쉴드가 없으면, 본체(Health)에 데미지를 줍니다.
		// ... (본체 데미지 로직)
	}
}

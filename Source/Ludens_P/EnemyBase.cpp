#include "EnemyBase.h"
#include "Net/UnrealNetwork.h"
#include "EnemyDescriptor.h"
#include "EnemyPoolManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AEnemyBase::AEnemyBase()
{
	bIsActiveInPool = false;
	PrimaryActorTick.bCanEverTick = false;
	CCC = CreateDefaultSubobject<UCreatureCombatComponent>(TEXT("CreatureCombat"));
}

void AEnemyBase::BeginPlay()
{
	// Descriptor와 CCC가 모두 유효할 때만 초기화 로직을 실행합니다.
	if (Descriptor && CCC)
	{
		// 이동속도 적용
		GetCharacterMovement()->MaxWalkSpeed = FMath::Max(120.f, Descriptor->WalkSpeed);
		// HP 적용
		CCC->InitStats(Descriptor->MaxHP);
	}

	Super::BeginPlay();

	if (CCC)
	{
		CCC->OnDied.AddDynamic(this, &AEnemyBase::HandleDied);
	}
}

void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 부모 클래스의 함수를 호출할 때 반드시 인자를 전달해야 합니다.
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 이제 이 클래스에서 복제할 변수를 등록합니다.
	DOREPLIFETIME(AEnemyBase, bIsActiveInPool);
}

// 1. 서버에서 호출되는 활성화 함수
void AEnemyBase::Activate(const FVector& Location, const FRotator& Rotation)
{
	if (!HasAuthority()) return; // 서버에서만 실행

	SetActorLocation(Location);
	SetActorRotation(Rotation);

	bIsActiveInPool = true;
	UpdateActiveState(true); // 서버에서도 직접 호출
}

// 2. 서버에서 호출되는 비활성화 함수
void AEnemyBase::Deactivate()
{
	if (!HasAuthority()) return; // 서버에서만 실행

	bIsActiveInPool = false;
	UpdateActiveState(false); // 서버에서도 직접 호출
}

// 3. 클라이언트에서 bIsActiveInPool 값이 복제 완료되면 자동으로 호출됨
void AEnemyBase::OnRep_IsActive()
{
	UpdateActiveState(bIsActiveInPool);
}

// 4. 실제 활성/비활성 로직 (서버와 클라이언트 모두에서 실행됨)
void AEnemyBase::UpdateActiveState(bool bNewIsActive)
{
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		if (bNewIsActive)
		{
			// 활성화될 때: 컴포넌트를 활성화하고 기본 이동 모드(예: 걷기)로 설정합니다.
			MoveComp->Activate();
			MoveComp->SetMovementMode(MOVE_Walking); 
		}
		else
		{
			// 비활성화될 때: 컴포넌트를 비활성화하고 이동 모드를 '없음'으로 설정하여
			// 중력을 포함한 모든 움직임을 완전히 정지시킵니다.
			MoveComp->Deactivate();
			MoveComp->SetMovementMode(MOVE_None);
		}
	}
	// 1. 보이기 / 숨기기 (가장 중요!)
	// bNewIsActive가 true일 때, !true = false가 되어 숨김 상태가 해제됩니다.
	SetActorHiddenInGame(!bNewIsActive);

	// 2. 충돌 켜기 / 끄기
	// 활성화될 때 충돌을 다시 켜서 바닥으로 떨어지지 않게 합니다.
	SetActorEnableCollision(bNewIsActive);

	// 3. 틱 켜기 / 끄기
	// 활성화될 때 틱을 다시 켜서 움직이거나 로직을 수행하게 합니다.
	SetActorTickEnabled(bNewIsActive);

	// 4. AI 컨트롤러 켜기 / 끄기 (AI로 움직이는 적에게 필수)
	AController* AIController = GetController();
	if (AIController)
	{
		if (bNewIsActive)
		{
			// AI의 모든 로직을 다시 시작시킵니다.
			// 아래는 예시이며, 사용하는 AI 클래스에 따라 다를 수 있습니다.
			// Cast<AAIController>(AIController)->BrainComponent->RestartLogic();
		}
		else
		{
			// AI의 모든 로직을 멈춥니다.
			// Cast<AAIController>(AIController)->BrainComponent->StopLogic("Deactivated by pool");
		}
	}

	// 기타 파티클, 사운드 등 필요한 컴포넌트들을 켜고 끄는 로직 추가...
}

bool AEnemyBase::IsActive() const
{
	return bIsActiveInPool;
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (HasAuthority() && CCC) {CCC->TakeDamage(DamageAmount);}
	return DamageAmount;
}

void AEnemyBase::HandleDied()
{
	// 사망 애니메이션, AI 중지, 캡슐 비활성화, 풀 반환
	// 죽는 처리는 서버에서만
	if (!HasAuthority())
	{
		return;
	}
    
	// 1. 레벨에 있는 PoolManager를 찾습니다.
	AEnemyPoolManager* PoolManager = Cast<AEnemyPoolManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyPoolManager::StaticClass()));

	if (PoolManager)
	{
		// 2. 자기 자신(this)을 풀로 반납해달라고 요청합니다.
		PoolManager->ReturnEnemy(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No PoolManager Found!!"));
		// 풀 매니저가 없는 예외적인 상황에서는 그냥 액터를 파괴합니다.
		Destroy();
	}
}
void AEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 이 액터가 파괴될 때, 그 이유를 로그로 출력합니다.
	// 이 로그는 문제 해결의 결정적인 단서가 될 것입니다.
	UE_LOG(LogTemp, Error, TEXT("ENEMY %s IS BEING DESTROYED! Reason: %s"), 
		*GetName(), 
		*UEnum::GetValueAsString(EndPlayReason));

	Super::EndPlay(EndPlayReason);
}

void AEnemyBase::Destroyed()
{
	// 누가 Destroy()를 호출했는지 정확히 알기 위해 로그를 남깁니다.
	UE_LOG(LogTemp, Error, TEXT("!!! %s's Destroyed() function was called !!!"), *GetName());
    
	// 로그를 남긴 후, 반드시 부모 클래스의 원래 함수를 호출해줘야 합니다.
	Super::Destroyed();
}
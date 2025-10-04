#include "EnemyBase.h"

#include "BrainComponent.h"
#include "EnemyAIController.h"
#include "Net/UnrealNetwork.h"
#include "EnemyDescriptor.h"
#include "EnemyHealthBarBase.h"
#include "EnemyPoolManager.h"
#include "PlayerStateComponent.h"
#include "ShieldComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "HitFeedbackComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "StealthComponent.h"
#include "Blueprint/UserWidget.h"

AEnemyBase::AEnemyBase()
{
	bIsActiveInPool = false;
	PrimaryActorTick.bCanEverTick = true;
	CCC = CreateDefaultSubobject<UCreatureCombatComponent>(TEXT("CreatureCombat"));
	ShieldComponent = CreateDefaultSubobject<UShieldComponent>(TEXT("Shield"));
	bReplicates = true;


	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(RootComponent);
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::World);
	HealthBarWidget->SetVisibility(false); // 처음에는 숨겨둡니다.

	HitFeedbackComponent = CreateDefaultSubobject<UHitFeedbackComponent>(TEXT("HitFeedbackComponent"));

	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarClassFinder(TEXT("/Game/Enemy/UI/WBP_EnemyHealthBar.WBP_EnemyHealthBar_C"));

	if (HealthBarClassFinder.Succeeded())
	{
		HealthBarWidget->SetWidgetClass(HealthBarClassFinder.Class);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("FATAL ERROR: Could not find WBP_EnemyHealthBar at the specified path!"));
	}
}
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	if (!HasAuthority())
	{
		if (CCC)
		{
			UE_LOG(LogTemp, Log, TEXT("[CLIENT TICK] Enemy: %s, CCC->CurrentHP: %f"), *GetName(), CCC->GetCurrentHP());
		}
	}
	*/
}

void AEnemyBase::EngageStealth_Implementation()
{
	IStealthInterface::EngageStealth_Implementation();
	UStealthComponent* StealthComponent = FindComponentByClass<UStealthComponent>();
	if (StealthComponent)
	{
		StealthComponent->EnterStealth();
	}
}

void AEnemyBase::DisengageStealth_Implementation()
{
	IStealthInterface::DisengageStealth_Implementation();
	UStealthComponent* StealthComponent = FindComponentByClass<UStealthComponent>();
	if (StealthComponent)
	{
		StealthComponent->ExitStealth();
	}
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	/*
	// 서버에서만 이 테스트를 실행합니다.
	if (HasAuthority())
	{
		
		// HitFeedbackComponent 클래스의 '설계도 원본(CDO)'을 직접 가져옵니다.
		UHitFeedbackComponent* DefaultComponent = UHitFeedbackComponent::StaticClass()->GetDefaultObject<UHitFeedbackComponent>();
        
		if (DefaultComponent && DefaultComponent->HitVFX)
		{
			// CDO에는 HitVFX가 유효하게 할당되어 있을 경우
			UE_LOG(LogTemp, Warning, TEXT("CDO CHECK: HitVFX is VALID on the Class Default Object."));
		}
		else
		{
			// CDO에서조차 HitVFX가 nullptr일 경우
			UE_LOG(LogTemp, Error, TEXT("CDO CHECK: HitVFX is NULL on the Class Default Object!"));
		}

		// 현재 이 액터 인스턴스가 가진 컴포넌트의 값도 확인합니다.
		if (HitFeedbackComponent && HitFeedbackComponent->HitVFX)
		{
			UE_LOG(LogTemp, Warning, TEXT("INSTANCE CHECK: HitVFX is VALID on this spawned actor instance."));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("INSTANCE CHECK: HitVFX is NULL on this spawned actor instance!"));
		}
	}
	*/
	
	// 3. "핵심 보험 코드": 현재 ColorType 값으로 색상을 한번 더 설정합니다.
	// OnRep이 먼저 실행되어 색 변경을 놓쳤더라도, BeginPlay가 끝나는 시점에
	// 최종적으로 올바른 색상을 보장해주는 매우 중요한 코드입니다.
	//SetBodyColor(ColorType);
	if (CCC)
	{
		CCC->OnHealthChanged.AddDynamic(this, &AEnemyBase::OnHealthUpdated);
	}
	if (ShieldComponent)
	{
		ShieldComponent->OnShieldsUpdated.AddDynamic(this, &AEnemyBase::OnShieldsUpdated);
	}
	if (HealthBarWidget)
	{
		HealthBarUI = Cast<UEnemyHealthBarBase>(HealthBarWidget->GetUserWidgetObject());
	}
	// Descriptor와 CCC가 모두 유효할 때만 초기화 로직을 실행합니다.
	if (Descriptor && CCC)
	{
		// 이동속도 적용
		GetCharacterMovement()->MaxWalkSpeed = Descriptor->WalkSpeed;
		// HP 적용
		CCC->InitStats(Descriptor->MaxHP);
	}
	if (CCC)
	{
		CCC->OnDied.AddDynamic(this, &AEnemyBase::HandleDied);
	}

}

void AEnemyBase::OnHealthUpdated(float NewCurrentHP, float NewMaxHP)
{
	// 위젯에 업데이트 명령을 내립니다.
	if (HealthBarUI && NewMaxHP > 0)
	{
		const float NewHealthPercent = NewCurrentHP / NewMaxHP;
		HealthBarUI->UpdateHealthBar(NewHealthPercent);
	}
}
void AEnemyBase::OnShieldsUpdated()
{
	if (HealthBarUI && ShieldComponent)
	{
		TArray<FShieldUIData> ShieldDataForUI;
		for (const FShieldLayer& Shield : ShieldComponent->GetActiveShields())
		{
			FShieldUIData Data;
			Data.ShieldColor = Shield.ShieldColor;
			Data.CurrentHealth = Shield.CurrentHealth;
			Data.MaxHealth = Shield.MaxHealth;
			ShieldDataForUI.Add(Data);
		}
		HealthBarUI->UpdateShields(ShieldDataForUI);
	}
}
void AEnemyBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 이제 이 클래스에서 복제할 변수를 등록
	DOREPLIFETIME(AEnemyBase, bIsActiveInPool);
	DOREPLIFETIME(AEnemyBase, ColorType);
	DOREPLIFETIME(AEnemyBase, bShouldShowUI);
}


// 1. 서버에서 호출되는 활성화 함수
void AEnemyBase::Activate(const FVector& Location, const FRotator& Rotation)
{
	if (!HasAuthority()) return; // 서버에서만 실행
	
	SetActorLocation(Location);
	SetActorRotation(Rotation);
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	bIsActiveInPool = true;

	if (SpawnVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpawnVFX, Location-FVector(0,0,150), Rotation, FVector(3));
	}
	GetWorld()->GetTimerManager().SetTimer(
		FinalizeSpawnTimerHandle,
		this,
		&AEnemyBase::FinalizeSpawn,
		2.0f,
		false);
	/*
	// --- 디버깅 코드 추가 ---
	if (USkeletalMeshComponent* MyMesh = GetMesh())
	{
		// 1. 액터의 최종 위치 (파란색 구)
		DrawDebugSphere(GetWorld(), GetActorLocation(), 30.f, 12, FColor::Blue, false, 5.0f);

		// 2. 소켓의 실제 월드 위치 (초록색 구)
		FVector SocketLocation = MyMesh->GetSocketLocation(TEXT("VFX"));
		DrawDebugSphere(GetWorld(), SocketLocation, 30.f, 12, FColor::Green, false, 5.0f);
	}
	// --- 디버깅 코드 끝 ---
	*/
}
void AEnemyBase::FinalizeSpawn()
{
	if (!HasAuthority()) return;

	// 모든 클라이언트에게 Mesh를 보여주라는 명령을 내립니다.
	Multicast_FinalizeSpawn();
}
void AEnemyBase::Multicast_FinalizeSpawn_Implementation()
{
	// ✨ Mesh를 보이게 하고 충돌을 켭니다.
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);

	bIsActiveInPool = true;
	// 기존의 활성화 로직을 여기서 호출하여 AI, 움직임 타이머 등을 시작시킵니다.
	UpdateActiveState(true); 

	// 서버라면 스탯 초기화 같은 게임 로직을 처리합니다.
	if (HasAuthority())
	{
		if (CCC && Descriptor)
		{
			CCC->InitStats(Descriptor->MaxHP);
		}
	}
}

// 2. 서버에서 호출되는 비활성화 함수
void AEnemyBase::Deactivate()
{
	if (!HasAuthority()) return; // 서버에서만 실행

	GetWorld()->GetTimerManager().ClearTimer(FinalizeSpawnTimerHandle);

	bIsActiveInPool = false;
	UpdateActiveState(false); // 서버에서도 직접 호출
	bShouldShowUI = false;
	// 이 적은 이제 데미지를 입은 적이 없음 (다음 재사용을 위해)
	bHasTakenDamage = false; 
	OnRep_ShouldShowUI();
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
	UpdateActiveState(false);
}

// 3. 클라이언트에서 bIsActiveInPool 값이 복제 완료되면 자동으로 호출됨
void AEnemyBase::OnRep_IsActive()
{
	UpdateActiveState(bIsActiveInPool);
}

// 4. 실제 활성/비활성 로직 (서버와 클라이언트 모두에서 실행됨)
void AEnemyBase::UpdateActiveState(bool bNewIsActive)
{
	if (bNewIsActive)
	{
		// 활성화될 때, 만약 MID가 아직 없다면(최초 스폰 또는 풀에서 재사용) 생성합니다.
		if (!BodyMID)
		{
			InitializeMID();
		}
	}
	else
	{
		// 비활성화될 때, 다음 재사용을 위해 포인터를 깔끔하게 정리합니다.
		BodyMID = nullptr;
	}
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	AController* AIController = GetController();
	if (bNewIsActive)
	{
		// 1. 즉시 처리할 내용들
		SetActorHiddenInGame(false);    // 보이기
		SetActorEnableCollision(true);  // 충돌 켜기
		SetActorTickEnabled(true);      // 틱 켜기
        
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(bShouldShowUI);
		}

		// 2. 움직임과 AI는 비활성화 상태로 시작
		if (MoveComp)
		{
			MoveComp->Deactivate(); // 아직 움직이지 않도록 비활성화
			MoveComp->SetMovementMode(MOVE_None);
		}
		if (AIController)
		{
			if (UBrainComponent* Brain = Cast<AEnemyAIController>(AIController)->BrainComponent)
			{
				Brain->StopLogic("Initial Spawn Delay");
			}
		}

		// 3. ✨ 타이머를 설정하여 일정 시간 후에 ActivateMovementAndAI 함수를 호출
		GetWorld()->GetTimerManager().SetTimer(
			SpawnDelayTimerHandle, 
			this, 
			&AEnemyBase::ActivateMovementAndAI, 
			SpawnMovementDelay, 
			false);
	}
	else // 비활성화 될 때
	{
		// ✨ 만약 스폰 지연 타이머가 돌고 있었다면 즉시 중지시킵니다.
		GetWorld()->GetTimerManager().ClearTimer(SpawnDelayTimerHandle);
        
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);

		if (MoveComp)
		{
			MoveComp->Deactivate();
			MoveComp->SetMovementMode(MOVE_None);
		}
		if (AIController)
		{
			if (UBrainComponent* Brain = Cast<AEnemyAIController>(AIController)->BrainComponent)
			{
				Brain->StopLogic("Deactivated by pool");
			}
		}
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}
	}
}

void AEnemyBase::ActivateMovementAndAI()
{
	// 서버인지 한번 더 확인 (안전장치)
	if (!HasAuthority()) return;

	// 캐릭터 무브먼트 컴포넌트를 활성화하고 걷기 모드로 설정
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->Activate();
		MoveComp->SetMovementMode(MOVE_Walking);
	}

	// AI 로직을 다시 시작
	if (AController* AIController = GetController())
	{
		if (UBrainComponent* Brain = Cast<AEnemyAIController>(AIController)->BrainComponent)
		{
			Brain->RestartLogic();
		}
	}
}
// 이 함수는 서버에서만 호출되어야 합니다.
void AEnemyBase::ChangeColorType(EEnemyColor NewColor)
{
	if (!HasAuthority()) return;
	
	ColorType = NewColor;
	OnRep_ColorType();

	if (ShieldComponent)
	{
		ShieldComponent->InitializeShields(NewColor);
	}
}

void AEnemyBase::HandleDeath_Implementation()
{
	HandleDied();
}

void AEnemyBase::OnRep_ShouldShowUI()
{
	if (HealthBarWidget)
	{
		// ✨ bShouldShowUI의 현재 값에 따라 UI를 켜거나 끕니다.
		HealthBarWidget->SetVisibility(bShouldShowUI);

		if (bShouldShowUI)
		{
			// 현재 체력 정보로 체력바 그리기
			if (CCC)
			{
				OnHealthUpdated(CCC->GetCurrentHP(), CCC->GetMaxHP());
			}
			// 현재 쉴드 정보로 쉴드바 그리기
			if (ShieldComponent)
			{
				OnShieldsUpdated();
			}
		}
	}
}

// ColorType 변수가 서버로부터 복제 완료되면 클라이언트에서 자동으로 호출됩니다.
void AEnemyBase::OnRep_ColorType()
{

	// SetBodyColor 함수를 호출하여 MID의 색상을 실제로 변경
	SetBodyColor(ColorType);

}

bool AEnemyBase::IsActive() const
{
	return bIsActiveInPool;
}

void AEnemyBase::SetBodyColor(EEnemyColor NewColor)
{
	if (!BodyMID)
	{
		UE_LOG(LogTemp, Warning, TEXT("BodyMID is not valid"));
		return;
	}
	FLinearColor TargetColor;
	switch (NewColor)
	{
	case EEnemyColor::Red:
		TargetColor = FLinearColor::Red;
		break;
	case EEnemyColor::Green:
		TargetColor = FLinearColor::Green;
		break;
	case EEnemyColor::Blue:
		TargetColor = FLinearColor::Blue;
		break;
	case EEnemyColor::Yellow:
		TargetColor = FLinearColor::Yellow;
		break;
	case EEnemyColor::Magenta:
		TargetColor = FLinearColor(1.f, 0.f, 1.f);
		break;
	case EEnemyColor::Cyan:
		TargetColor = FLinearColor(0.f,1.f,1.f);
		break;
	case EEnemyColor::Black:
	default:
		TargetColor = FLinearColor::Black;
		break;
	}
	BodyMID->SetVectorParameterValue(TEXT("BodyColor"), TargetColor);
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!bShouldShowUI)
	{
		bShouldShowUI = true;
	}
	// 위젯 컴포넌트가 안보일시 보이게
	if (HealthBarWidget && !HealthBarWidget->IsVisible())
	{
		HealthBarWidget->SetVisibility(true);
		if (ShieldComponent) OnShieldsUpdated();
		
	}
	// 로직을 이런식으로 짜면 원래 안됨
	// 원래는 HasAuthority로 서버에서만 해야되지만 버그 해결 실패로 임시방편임 
	if (!HasAuthority()) return 0.f;
	// ----------------------------쉴드 체크 로직-----------------------------------
	if (ShieldComponent && !ShieldComponent->AreAllShieldsBroken())
	{
		EEnemyColor DamageColor = EEnemyColor::Black;

		// 1. 공격자 컨트롤러가 있는지 확인
		if (EventInstigator)
		{
			// 2. 컨트롤러가 조종하는 폰(캐릭터)이 있는지 확인
			APawn* InstigatorPawn = EventInstigator->GetPawn();
			if (InstigatorPawn)
			{
				// 3. 폰에 PlayerStateComponent가 있는지 확인
				// 이거를 확인하는 이유는 플레이어의 색상에 접근하기 위해
				UPlayerStateComponent* InstigatorStateComp = InstigatorPawn->FindComponentByClass<UPlayerStateComponent>();
				if (InstigatorStateComp)
				{
					// 모든 것이 유효할 때만 PlayerColor에 접근
					DamageColor = InstigatorStateComp->PlayerColor;
				}
			}
		}
        
		ShieldComponent->TakeShieldDamage(DamageAmount, DamageColor);
	}
	//활성화된 쉴드 없으면 직접 데미지 주기
	else
	{
		if (CCC)
			CCC->TakeDamage(DamageAmount);
	}
	FVector Loc = GetActorLocation();

	/*
	 * HitFeedbackComp Nullptr 에러 해결하기
	 */
	
	HitFeedbackComponent->PlayHitEffects(Loc);
	
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
    
	// 1. 레벨에 있는 PoolManager를 찾기
	AEnemyPoolManager* PoolManager = Cast<AEnemyPoolManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyPoolManager::StaticClass()));

	if (PoolManager)
	{
		// 2. 자기 자신을 풀로 반환을 요청
		PoolManager->ReturnEnemy(this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No PoolManager Found!!"));
		// 풀 매니저가 없는 예외 케이스 경우 그냥 액터를 파괴
		Destroy();
	}
}

void AEnemyBase::InitializeMID()
{
	if (GetMesh())
	{
		// 0번 슬롯에 대한 MID 생성
		BodyMID = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);

		// 생성 성공 여부 로그로 확인
		if (BodyMID)
		{
			// GetName()으로 어떤 적의 MID가 생성되었는지 명확히 확인
			UE_LOG(LogTemp, Warning, TEXT("[%s] MID created successfully."), *GetName());

			SetBodyColor(ColorType);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[%s] FAILED to create MID. Check material slot 0 on the mesh assigned in the Blueprint."), *GetName());
		}
	}
}

void AEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AEnemyBase::Destroyed()
{
	Super::Destroyed();
}

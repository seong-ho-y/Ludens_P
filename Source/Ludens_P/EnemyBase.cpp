#include "EnemyBase.h"

#include "BrainComponent.h"
#include "DeColorProjec.h"
#include "EnemyAIController.h"
#include "Net/UnrealNetwork.h"
#include "EnemyDescriptor.h"
#include "EnemyHealthBarBase.h"
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
#include "Ludens_PGameMode.h"
#include "EnemyPoolManager.h"
#include "GrenadeProjectile.h"

struct FEnemySpawnProfile;

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
	GetMesh()->SetIsReplicated(true);
	// 복제될 ColorType의 기본값을 '초기화 안됨' 상태로 지정
	ColorType = EEnemyColor::Uninitialized; 
	
}
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

void AEnemyBase::SetStealthAmount(float X)
{
	if (HasAuthority()) // 값 변경은 서버에서만!
	{
		StealthAmount = X;
		// 서버에서도 바로 적용되게 하려면 수동 호출
		OnRep_StealthAmount(); 
	}
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	if (CCC)
	{
		CCC->OnHealthChanged.AddDynamic(this, &AEnemyBase::OnHealthUpdated);
		CCC->OnDied.AddDynamic(this, &AEnemyBase::HandleDied);
	}
	if (ShieldComponent)
	{
		ShieldComponent->OnShieldsUpdated.AddDynamic(this, &AEnemyBase::OnShieldsUpdated);
	}
	if (HealthBarWidget)
	{
		HealthBarUI = Cast<UEnemyHealthBarBase>(HealthBarWidget->GetUserWidgetObject());
	}

}

void AEnemyBase::InitializeEnemy(const FEnemySpawnProfile& Profile)
{
	PlaySpawnVFX();

	if (!HasAuthority()) return;
	if (!Descriptor || !CCC) return;

	// 1. 기본 스탯을 항상 Descriptor에서 먼저 가져옵니다. (스탯 리셋 효과)
	float FinalMaxHP = Descriptor->MaxHP;
	float FinalWalkSpeed = Descriptor->WalkSpeed;
	float FinalMaxShield = Descriptor->MaxShield;

	// 2. 프로필에 강화형 스탯(StatDataAsset)이 있다면 보정치를 적용합니다.
	if (Profile.StatDataAsset)
	{
		FinalMaxHP *= Profile.StatDataAsset->HealthMultiplier;
		FinalWalkSpeed *= Profile.StatDataAsset->SpeedMultiplier;
		FinalMaxShield *= Profile.StatDataAsset->ShieldMultiplier;
	}

	// 3. 최종 계산된 스탯을 각 컴포넌트에 적용합니다.
	GetCharacterMovement()->MaxWalkSpeed = FinalWalkSpeed;
	CCC->InitStats(FinalMaxHP); // CCC의 InitStats가 최대 체력과 현재 체력을 모두 설정해준다고 가정
	ShieldComponent->DefaultShieldHealth = FinalMaxShield;

	// 4. 색깔 등 외형을 변경합니다.
	// 2. 색깔에 맞춰 머티리얼을 바꿉니다.
	// 메시 컴포넌트의 다이내믹 머티리얼 인스턴스를 생성하고,
	// "BodyColor" 같은 벡터 파라미터의 값을 바꿔주면 됩니다.
	ChangeColorType(Profile.Color);
	

	// [가져온 로직 2] AI 및 움직임 지연 활성화 (서버에서만 타이머 설정)
	if (HasAuthority())
	{
		// 처음엔 AI와 움직임을 끈 상태로 시작
		GetCharacterMovement()->Deactivate();
		if (AController* AIController = GetController())
		{
			if (UBrainComponent* Brain = Cast<AEnemyAIController>(AIController)->BrainComponent)
			{
				Brain->StopLogic("Initial Spawn Delay");
			}
		}

		// 타이머를 설정하여 2초 뒤에 AI와 움직임을 활성화
		GetWorld()->GetTimerManager().SetTimer(
			  SpawnDelayTimerHandle, 
			  this, 
			  &AEnemyBase::ActivateMovementAndAI, 
			  2.0f, // 2초 지연 (원하는 시간으로 조절)
			  false);
	}
}

void AEnemyBase::PlaySoundAtEnemy(USoundBase* Sound)
{
	if (Sound && GetWorld())
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation());
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
	// StealthAmount 변수를 복제 리스트에 추가
	DOREPLIFETIME(AEnemyBase, StealthAmount);
}

void AEnemyBase::OnRep_StealthAmount()
{
	// 실제 머티리얼 업데이트 로직 호출
	UpdateStealthMaterial();
}

// 실제 머티리얼 파라미터를 변경하는 함수
void AEnemyBase::UpdateStealthMaterial()
{
	if (StealthMID)
	{
		// 복제된 StealthAmount 값을 사용해 파라미터 설정
		StealthMID->SetScalarParameterValue(TEXT("StealthAmount"), StealthAmount);
	}
}

void AEnemyBase::SpawnDeadVFX_Implementation()
{
	if (DeadVFX) UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DeadVFX, GetActorLocation());
}


// 2. 서버에서 호출되는 비활성화 함수
void AEnemyBase::Deactivate()
{
	if (!HasAuthority()) return; // 서버에서만 실행

	GetWorld()->GetTimerManager().ClearTimer(FinalizeSpawnTimerHandle);
	// 1. AnimInstance를 가져와서 변수에 저장합니다.

	// 2. AnimInstance가 유효한 포인터인지(nullptr이 아닌지) 확인합니다.
    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        // 3. 유효할 때만 Montage_Stop을 호출합니다.
        AnimInstance->Montage_Stop(0.1f);
    }

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


void AEnemyBase::OnRep_IsActive()
{
	UpdateActiveState(bIsActiveInPool);
}


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

void AEnemyBase::PlaySpawnVFX_Implementation()
{
	if (SpawnVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), SpawnVFX, GetActorLocation());
	}
	PlaySoundAtEnemy(SpawnSound);
}

FLinearColor AEnemyBase::GetColorValue(EEnemyColor Color) const
{
	switch (Color)
	{
	case EEnemyColor::Red:
		return FLinearColor::Red;
	case EEnemyColor::Green:
		return FLinearColor::Green;
	case EEnemyColor::Blue:
		return FLinearColor::Blue;
	case EEnemyColor::Yellow:
		return FLinearColor::Yellow;
	case EEnemyColor::Magenta:
			return FLinearColor(1.0f, 0.0f, 1.0f); // R=1, G=0, B=1
	case EEnemyColor::Cyan:
		return FLinearColor(0.0f, 1.0f, 1.0f); // R=0, G=1, B=1
	case EEnemyColor::Black:
		return FLinearColor::Black;
	default:
		// 혹시 모를 예외 상황을 대비해 기본값(흰색)을 반환합니다.
			UE_LOG(LogTemp, Error, TEXT("GetColorValue return White!!"))
			return FLinearColor::White;
	}
}

void AEnemyBase::PlayCastMontage_Implementation()
{
	if (!HasAuthority()) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && CastMontage)
	{
		AnimInstance->Montage_Play(CastMontage);
	}
}

void AEnemyBase::PlayShootMontage_Implementation()
{
	if (!HasAuthority()) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ShootMontage)
	{
		AnimInstance->Montage_Play(ShootMontage);
	}
}

void AEnemyBase::PlayDashEffects_Implementation()
{
	// ✨ 이 함수는 서버와 모든 클라이언트에서 직접 실행됩니다.
    
	// 1. 몽타주 재생
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DashMontage)
	{
		if (!AnimInstance->IsAnyMontagePlaying())
		AnimInstance->Montage_Play(DashMontage);
	}

	// 2. VFX 재생
	if (DashVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), DashVFX, GetActorLocation());
	}
}

// '공격' Multicast 함수의 실제 내용
void AEnemyBase::PlayAttackMontage_Implementation()
{
	// ✨ 이 함수도 서버와 모든 클라이언트에서 직접 실행됩니다.

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && MeleeAttackMontage)
	{
		if (!AnimInstance->IsAnyMontagePlaying())
		AnimInstance->Montage_Play(MeleeAttackMontage);
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
	if (ColorType == EEnemyColor::Uninitialized)
	{
		UE_LOG(LogTemp, Error, TEXT("OnRep_ColorType : White"));
		return;
	}
	InitializeMID();
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
		UE_LOG(LogTemp, Error, TEXT("BodyMID is not valid"));
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
		//UE_LOG(LogTemp, Warning, TEXT("ShieldComponent damageStart"));
		EEnemyColor DamageColor = EEnemyColor::Black;

		// 1-1. 공격자가 수류탄인지 확인
		if (DamageCauser && DamageCauser->IsA<AGrenadeProjectile>())
		{
			// 수류탄이라면 DamageColor Black으로 유지
			//UE_LOG(LogTemp, Warning, TEXT("Damage from Grenade detected! Setting color to Black."));
		}
		// 1-2. 공격자 컨트롤러가 있는지 확인
		else if (EventInstigator)
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
					// 임시로 Red 넣고 테스트해볼거임 <- 탈색제 테스트
					//DamageColor = EEnemyColor::Red;
				}
			}
		}
		//UE_LOG(LogTemp,Error, TEXT("DamageAmount  %f"), DamageAmount)
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
	PlaySoundAtEnemy(HitSound);
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
    // 0. 적이 죽었음을 방송하기 (EnemyCount 세는 용도)
	OnEnemyDied.Broadcast(this);

	PlaySoundAtEnemy(DeathSound);
	// 0-1. 죽었을 때 VFX
	SpawnDeadVFX();
	
	// 1. 레벨에 있는 PoolManager를 찾기

	if (AEnemyPoolManager* PoolManager = Cast<AEnemyPoolManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyPoolManager::StaticClass())))
	{
		// 2. 자기 자신을 풀로 반환을 요청
		PoolManager->ReturnEnemy(this);
		//UE_LOG(LogTemp, Log, TEXT("Enemy Died and Return to Pool"))
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No PoolManager Found!!"));
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
			//UE_LOG(LogTemp, Warning, TEXT("[%s] MID created successfully."), *GetName());

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

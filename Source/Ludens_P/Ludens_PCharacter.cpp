// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ludens_PCharacter.h"

#include "EnemyBase.h"
#include "EngineUtils.h"
#include "Ludens_PProjectile.h"
#include "Animation/AnimInstance.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PlayerAttackComponent.h"
#include "PlayerStateComponent.h"
#include "Engine/LocalPlayer.h"
<<<<<<< Updated upstream
#include "Util/ColorConstants.h"
#include "Net/UnrealNetwork.h"
=======
#include "RewardSystemComponent.h"
>>>>>>> Stashed changes

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ALudens_PCharacter

ALudens_PCharacter::ALudens_PCharacter()
{
	//멀티 설정
	bReplicates = true;
	SetReplicatingMovement(true);
	
	//bReplicateMovement = true;
	//필드가 private으로 되어있어서 SetReplciatingMovemt() Setter함수로 접근 및 설정

	
	//무기 컴포넌트 할당
	Weapon = CreateDefaultSubobject<UTP_WeaponComponent>(TEXT("WeaponComponent"));
	Weapon->SetupAttachment(RootComponent);
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
	
	// 카메라 컴포넌트 생성 및 할당
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // 카메라 위치 조정
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	// 기본 메시 설정 (다른 사람이 보는 메시)
	GetMesh()->SetOnlyOwnerSee(false); // 모든 사람이 보도록
	GetMesh()->SetIsReplicated(true);
	GetMesh()->SetupAttachment(GetCapsuleComponent());
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -96.f)); // 캡슐 기준 정렬
	GetMesh()->SetRelativeRotation(FRotator(0.f, 0.f, 0.f)); // 필요 시 방향 조절

<<<<<<< Updated upstream
	JumpCount = 0; // Default 점프 수 설정
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 1.0f;
	OriginalGroundFriction = 8.0f;
	OriginalBrakingDeceleration = 2048.0f;

	// 네트워크 업데이트 주기 향상
	NetUpdateFrequency = 100.0f;
	MinNetUpdateFrequency = 50.0f;

	// 이동 컴포넌트 복제 설정
	GetCharacterMovement()->SetIsReplicated(true);

	// 초기 탄알 수 설정
	CurrentAmmo = MaxAmmo;
=======
	RewardSystem = CreateDefaultSubobject<URewardSystemComponent>(TEXT("RewardSystem"));
>>>>>>> Stashed changes
}

void ALudens_PCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
		/*
		if (Controller)
		{
			UE_LOG(LogTemp, Log, TEXT("✅ Controller is %s"), *Controller->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("❌ No Controller assigned to %s"), *GetName());
		}
		*/

	// 로컬 플레이어 컨트롤러 확인
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// 기본 입력 매핑 컨텍스트 추가
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	//컴포넌트 할당
	PlayerAttackComponent = FindComponentByClass<UPlayerAttackComponent>();
	PlayerStateComponent = FindComponentByClass<UPlayerStateComponent>();

	if (!DashAction)
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("DashAction is null!"));
	}
	else if (!MeleeAttackAction)
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("MeleeAttackAction is null!"));
	}
	else if (!PlayerAttackComponent)
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("PlayerAttackComponent is null!"));
	}
	else if (!PlayerStateComponent)
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("PlayerStateComponent is null!"));
	}
}
void ALudens_PCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//적의 TakeDamage 메서드 실행 확인 여부
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC && PC->IsInputKeyDown(EKeys::E))
	{
		for (TActorIterator<AEnemyBase> It(GetWorld()); It; ++It)
		{
			AEnemyBase* Enemy = *It;
			if (Enemy && Enemy->Combat)
			{
				Enemy->Combat->TakeDamage(10);
				break;
			}
		}
	}
}



//////////////////////////////////////////////////////////////////////////// Input

void ALudens_PCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping 여기다가 Double Jumping으로 수정
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALudens_PCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALudens_PCharacter::Look);
		
		/*//Fire 입력
		PlayerInputComponent->BindAction("Fire", IE_Pressed, Weapon, &UTP_WeaponComponent::Fire);*/

		// Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ALudens_PCharacter::Dash);

		// MeleeAttack
		EnhancedInputComponent->BindAction(MeleeAttackAction, ETriggerEvent::Started, this, &ALudens_PCharacter::MeleeAttack);

		// TestAttack -> P
		EnhancedInputComponent->BindAction(TestAttackAction, ETriggerEvent::Started, this, &ALudens_PCharacter::TestAttack);

		// Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ALudens_PCharacter::Reload);

		// Fire -> Enhanced Input
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ALudens_PCharacter::Fire);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void ALudens_PCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ALudens_PCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ALudens_PCharacter::TestAttack(const FInputActionValue& Value)
{
	if (PlayerStateComponent)
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("TestAttack!"));
		PlayerStateComponent->TakeDamage(100.0f);
	}
}

void ALudens_PCharacter::Jump()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_Jump();
		return;
	}

	if (JumpCount < MaxJumpCount)
	{
		Super::Jump();
		JumpCount++;
		if (JumpCount <= MaxJumpCount)
		{
			LaunchCharacter(FVector(0,0,600), false, true);
		}
	}
}

// 서버 전용 점프 처리
void ALudens_PCharacter::Server_Jump_Implementation()
{
	Jump();
}


void ALudens_PCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	if (GetLocalRole() == ROLE_Authority)
	{
		JumpCount = 0; // 서버에서 JumpCount 리셋
	}
}

void ALudens_PCharacter::Dash(const FInputActionValue& Value)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_Dash();
		return;
	}
	
	if (bCanDash && CurrentDashCount > 0)
	{
		UCharacterMovementComponent* MoveComp = GetCharacterMovement();
		if (!MoveComp) return;

		GetWorld()->GetTimerManager().ClearTimer(DashTimerHandle); // 기존에 돌아가던 타이머 취소
	
		// 1. 원본 값 백업
		OriginalGroundFriction = MoveComp->GroundFriction;
		OriginalBrakingDeceleration = MoveComp->BrakingDecelerationWalking;

		// 2. 대시 중 마찰력 제거
		MoveComp->GroundFriction = 1.0f;
		MoveComp->BrakingDecelerationWalking = 1.0f;

		// 3. 대시 방향 계산
		FVector DashDirection = MoveComp->Velocity;
		DashDirection.Z = 0;
		DashDirection.Normalize();
		if (DashDirection.IsNearlyZero())
		{ 
			DashDirection = GetActorForwardVector();
		}

		if (CurrentDashCount <= 0) return;

		// 서버에서 강제 실행
		LaunchCharacter(DashDirection * DashSpeed, true, true);
		
		CurrentDashCount--;
		bCanDash = false;
	
		// 5. 0.2초 후 원래 값 복원 (대시 지속시간에 맞게 조절)
		GetWorld()->GetTimerManager().SetTimer(
			DashPhysicsTimerHandle,
			[this]()
			{
				if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
				{
					MoveComp->GroundFriction = OriginalGroundFriction;
					MoveComp->BrakingDecelerationWalking = OriginalBrakingDeceleration;
				}
			},
			0.2f,
			false
		);

		// 1초 후 다음 대쉬 가능
		GetWorld()->GetTimerManager().SetTimer(
			DashCooldownTimerHandle,
			[this]() { bCanDash = true; },
			DashCooldown,
			false
		);

		//3초마다 대쉬 충전
		if (!GetWorld()->GetTimerManager().IsTimerActive(DashRechargeTimerHandle))
		{
			GetWorld()->GetTimerManager().SetTimer(
				DashRechargeTimerHandle,
				this,
				&ALudens_PCharacter::RechargeDash,
				DashRechargeTime,
				true
			);
		}
	}
}

void ALudens_PCharacter::Server_Dash_Implementation()
{
	Dash(FInputActionValue()); // 실제 대쉬 실행
}

void ALudens_PCharacter::RechargeDash()
{
	if (GetLocalRole() == ROLE_Authority && CurrentDashCount < MaxDashCount)
	{
		CurrentDashCount++;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(DashRechargeTimerHandle);
	}
}

void ALudens_PCharacter::ResetMovementParams() const
{
	// 대쉬 끝난 뒤 마찰력 초기화
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->GroundFriction = OriginalGroundFriction;
		MoveComp->BrakingDecelerationWalking = OriginalBrakingDeceleration;
	}
}

void ALudens_PCharacter::MeleeAttack(const FInputActionValue& Value)
{
	//근접 공격 로직 호출
	if (PlayerAttackComponent)
	{
		PlayerAttackComponent->TryMeleeAttack();
	}
}

void ALudens_PCharacter::Server_Fire_Implementation(const FInputActionValue& Value)
{
	Fire(Value); // 서버에서 발사 처리
}

void ALudens_PCharacter::Fire(const FInputActionValue& Value)
{
	// 무기 공격 로직 호출
	if (GetLocalRole() < ROLE_Authority)
	{
		// 클라: 서버에 발사 요청 RPC 호출
		Server_Fire(Value);
		return;
	}

	// 서버: 실제 발사 처리
	if (CurrentAmmo > 0)
	{
		CurrentAmmo--;
		PlayerAttackComponent->TryWeaponAttack();
	}
}

void ALudens_PCharacter::Server_Reload_Implementation()
{
	Reload(FInputActionValue());
}

void ALudens_PCharacter::Reload(const FInputActionValue& Value)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_Reload();
		return;
	}

	if (SavedAmmo <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Case1: Saved Ammo is 0"));
		return;
	}
	else if (SavedAmmo - (MaxAmmo - CurrentAmmo) <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Case2: Left Ammo Reloaded"));
		CurrentAmmo += SavedAmmo;
		SavedAmmo = 0;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Case3: Reload Complete"));
		SavedAmmo -= (MaxAmmo-CurrentAmmo);
		CurrentAmmo = MaxAmmo;
	}
	UE_LOG(LogTemp, Warning, TEXT("Current Ammo %d"), CurrentAmmo);
}

void ALudens_PCharacter::OnRep_SavedAmmo()
{
	// 젤루 흡수 시 변경되는 UI, 사운드 등
}

void ALudens_PCharacter::OnRep_CurrentAmmo()
{
	// 재장전 시 변경되는 UI, 사운드 등
}

int16 ALudens_PCharacter::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

void ALudens_PCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALudens_PCharacter, JumpCount);
	DOREPLIFETIME(ALudens_PCharacter, CurrentDashCount);
	DOREPLIFETIME(ALudens_PCharacter, bCanDash);
	DOREPLIFETIME(ALudens_PCharacter, SavedAmmo);
	DOREPLIFETIME(ALudens_PCharacter, CurrentAmmo);
	DOREPLIFETIME(ALudens_PCharacter, bCanReload);

}
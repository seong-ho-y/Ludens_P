// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ludens_PCharacter.h"
#include "Ludens_PProjectile.h"
#include "Blueprint/UserWidget.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "RewardSystemComponent.h"
#include "PlayerAttackComponent.h"
#include "PlayerStateComponent.h"
#include "TP_WeaponComponent.h"
#include "WeaponAttackHandler.h"
#include "CreatureCombatComponent.h"
#include "JellooComponent.h"
#include "PlayerState_Real.h"
#include "ReviveComponent.h"
#include "LudensAppearanceData.h"

#include "Engine/LocalPlayer.h"
#include "Net/UnrealNetwork.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ALudens_PCharacter

ALudens_PCharacter::ALudens_PCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	RewardSystem = CreateDefaultSubobject<URewardSystemComponent>(TEXT("RewardSystem"));
	
	// GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 1.0f;
	OriginalGroundFriction = 8.0f;
	OriginalBrakingDeceleration = 2048.0f;

	// 네트워크 업데이트 주기 향상
	NetUpdateFrequency = 100.0f;
	MinNetUpdateFrequency = 50.0f;

	// 이동 컴포넌트 복제 설정
	GetCharacterMovement()->SetIsReplicated(true);

	// 나이아가라 컴포넌트 생성 및 설정
	DashNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DashNiagaraComponent"));
    
	// 메시 컴포넌트에 부착
	DashNiagaraComponent->SetupAttachment(GetMesh()); 
    
	// **가장 중요: 자동 활성화 비활성화**
	// 블루프린트에서 Set Active로 제어하기 위해 기본적으로 꺼둡니다.
	DashNiagaraComponent->bAutoActivate = false;
}

void ALudens_PCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();


	//컴포넌트 할당
	PlayerAttackComponent = FindComponentByClass<UPlayerAttackComponent>();
	PlayerStateComponent = FindComponentByClass<UPlayerStateComponent>();
	WeaponComponent = FindComponentByClass<UTP_WeaponComponent>();
	ReviveComponent = FindComponentByClass<UReviveComponent>();

	if (PlayerAttackComponent && PlayerAttackComponent->WeaponAttackHandler && WeaponComponent)
	{
		PlayerAttackComponent->WeaponAttackHandler->WeaponComp = WeaponComponent;
	}

	if (!PlayerAttackComponent) { UE_LOG(LogTemplateCharacter, Error, TEXT("PlayerAttackComponent is null!")); }
	if (!PlayerStateComponent) { UE_LOG(LogTemplateCharacter, Error, TEXT("PlayerStateComponent is null!")); }
	if (!WeaponComponent) { UE_LOG(LogTemplateCharacter, Error, TEXT("WeaponComponent is null!")); }
	if (!ReviveComponent) { UE_LOG(LogTemplateCharacter, Error, TEXT("ReviveComponent is null!")); }


	// 로컬 플레이어 컨트롤러 확인
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LP = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
			{
				if (DefaultMappingContext)
				{
					Subsys->AddMappingContext(DefaultMappingContext, 0);
				}
				else
				{
					UE_LOG(LogTemplateCharacter, Error, TEXT("❌ DefaultMappingContext is NULL. Set it on BP_FirstPersonPlayerController or Character BP."));
				}
			}
		}

		/*
		//컴포넌트 할당
		PlayerAttackComponent = FindComponentByClass<UPlayerAttackComponent>();
		PlayerStateComponent = FindComponentByClass<UPlayerStateComponent>();
		WeaponComponent = FindComponentByClass<UTP_WeaponComponent>();
		ReviveComponent = FindComponentByClass<UReviveComponent>();

		if (PlayerAttackComponent && WeaponComponent)
		{
			PlayerAttackComponent->WeaponAttackHandler->WeaponComp = WeaponComponent;
		}

		*/

		// --- 널 체크 ---
		if (!DashAction) { UE_LOG(LogTemplateCharacter, Error, TEXT("DashAction is null!")); }
		if (!MeleeAttackAction) { UE_LOG(LogTemplateCharacter, Error, TEXT("MeleeAttackAction is null!")); }
		


		// 로비 UI 모드 잔상이 있으면 입력이 막힐 수 있음 → 게임 전용으로 전환
		PC->SetInputMode(FInputModeGameOnly{});
		PC->bShowMouseCursor = false;
	}
}

void ALudens_PCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// PlayerState가 아직 할당되지 않았을 때만 할당 시도
	if (!bPSRInitialized)
	{
		PSR = Cast<APlayerState_Real>(GetPlayerState());
		if (PSR) // 115 번째 줄
		{
			MaxDashCount = PSR->MaxDashCount;
			CurrentDashCount = MaxDashCount;
			CurrentAmmo = PSR->MaxAmmo;
			MaxSavedAmmo = PSR->MaxSavedAmmo;
			SavedAmmo = MaxSavedAmmo / 2;
			MaxAmmo = PSR->MaxAmmo;
			CurrentAmmo = MaxAmmo;

			// 외형/색 1회 적용
			if (!bCosmeticsApplied && AppearanceDB && Mesh1P)
			{
				// 로비에서 선택한 외형/색을 1회 반영

				// 변경: EEnemyColor → ELobbyColor 매핑 1줄 추가
				auto ToLobbyColor = [](EEnemyColor C)->ELobbyColor
					{
						switch (C)
						{
						case EEnemyColor::Red:   return ELobbyColor::Red;
						case EEnemyColor::Green: return ELobbyColor::Green;
						case EEnemyColor::Blue:  return ELobbyColor::Blue;
						default:                 return ELobbyColor::Red;
						}
					};

				if (AppearanceDB && GetMesh() && PSR)
				{
					AppearanceDB->ApplyToByEnemyColor(GetMesh(), PSR->AppearanceId, PSR->PlayerColor);
					UE_LOG(LogTemp, Display, TEXT("[Cosmetics] Body applied Ap=%d, EnemyColor=%d, Mesh=%s"),
						PSR->AppearanceId, (int)PSR->PlayerColor, *GetNameSafe(GetMesh()));
				}


				bCosmeticsApplied = true;
			}

			// 스킬 선택값 캐시(스킬 자체 로직은 아직 미구현이므로 보관만)
			CachedSubskillId = PSR->SubskillId;

			bPSRInitialized = true;  // 한 번만 실행되도록
			UE_LOG(LogTemplateCharacter, Log, TEXT("PSR Completed in Ludens_PCharacter!"))
		}
	}
	// 다른 Tick 로직에서도 PSR을 접근하는 경우
	if (!PSR) return;
    
	// 이후 안전하게 PSR 멤버 사용 가능
}

void ALudens_PCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ALudens_PCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALudens_PCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALudens_PCharacter::Look);

		// Dash
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ALudens_PCharacter::Dash);

		// MeleeAttack
		EnhancedInputComponent->BindAction(MeleeAttackAction, ETriggerEvent::Started, this, &ALudens_PCharacter::Interact);

		// TestAttack -> P
		EnhancedInputComponent->BindAction(TestAttackAction, ETriggerEvent::Started, this, &ALudens_PCharacter::TestAttack);

		// Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &ALudens_PCharacter::Reload);

		// Fire
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &ALudens_PCharacter::Fire);

		// Revive
		EnhancedInputComponent->BindAction(ReviveAction, ETriggerEvent::Started, this, &ALudens_PCharacter::Interact);

		// Absorb
		EnhancedInputComponent->BindAction(AbsorbAction, ETriggerEvent::Ongoing, this, &ALudens_PCharacter::Absorb);
		EnhancedInputComponent->BindAction(AbsorbAction, ETriggerEvent::Completed, this, &ALudens_PCharacter::AbsorbComplete);
	}
}


void ALudens_PCharacter::Move(const FInputActionValue& Value)
{
	if (PlayerStateComponent->IsDead) return;
	if (ReviveComponent && ReviveComponent->IsReviving())
	{
		ReviveComponent->CancelRevive(); // ← ReviveTimer 해제 + KnockedTimer 재개
	}
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
	if (PlayerStateComponent->IsDead) return;
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
		PlayerStateComponent->TakeDamage(100.0f);
	}
}

void ALudens_PCharacter::Jump()
{
	if (PlayerStateComponent->IsDead || PlayerStateComponent->IsKnocked) return;
	
	if (GetLocalRole() < ROLE_Authority)
	{
		Server_Jump();
		return;
	}
	
	if (ReviveComponent && ReviveComponent->IsReviving())
	{
		ReviveComponent->CancelRevive(); // ← ReviveTimer 해제 + KnockedTimer 재개
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
	if (PlayerStateComponent->IsDead || PlayerStateComponent->IsKnocked) return;
	if (ReviveComponent && ReviveComponent->IsReviving())
	{
		ReviveComponent->CancelRevive(); // ← ReviveTimer 해제 + KnockedTimer 재개
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
		// Multicast로 이펙트 활성화 명령 전달**
		// 서버에서 이 함수를 호출하면, 모든 클라이언트(서버 포함)에서 MulticastControlDashEffect_Implementation이 실행됩니다.
		MulticastControlDashEffect(true); 
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
		
		// 1. 서버 시간 복제
		// 현재 서버 시간을 복제 변수에 저장합니다.
		ReplicatedDashCooldownStartTime = GetWorld()->GetTimeSeconds(); 
        
		// 2. OnRep 함수가 서버에서도 실행되도록 수동 호출
		// 서버의 UI도 업데이트해야 하므로 필수
		OnRep_DashCooldownTime(); 

		// 3. 쿨타임 종료 로직은 서버 타이머로 유지
		// bCanDash를 True로 바꾸는 로직은 서버에서만 결정해야 합니다.
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
				PSR->DashRechargeTime,
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
	if (GetLocalRole() == ROLE_Authority && CurrentDashCount < PSR->MaxDashCount)
	{
		CurrentDashCount++;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(DashRechargeTimerHandle);
	}
}

void ALudens_PCharacter::OnRep_DashCooldownTime()
{
	// 이 함수는 클라이언트와 서버에서 모두 실행됩니다.
	// **클라이언트 UI**가 쿨타임 시작을 알 수 있게 됩니다.
    
	// 이 로직은 UI 바인딩에 필요한 정보를 제공하는 역할을 합니다.
	// UI 바인딩 로직에서 이 변수를 사용하게 됩니다.
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

void ALudens_PCharacter::Interact(const FInputActionValue& Value) // 앞에 있는 대상이 무엇인지 판별해주는 메서드
{
	if (PlayerStateComponent->IsDead || PlayerStateComponent->IsKnocked) return;
	if (ReviveComponent && ReviveComponent->IsReviving())
	{
		ReviveComponent->CancelRevive(); // ← ReviveTimer 해제 + KnockedTimer 재개
	}
	// 라인 트레이스를 통해 앞에 있는 대상이 무엇인지 판별
	// 화면 중심에서 월드 방향 구하기
	FVector WorldLocation = FirstPersonCameraComponent->GetComponentLocation();
	FRotator CameraRotation = GetActorRotation();
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		CameraRotation = PC->PlayerCameraManager->GetCameraRotation();
	}
	else UE_LOG(LogTemp, Warning, TEXT("❗ GetController() is null, fallback to actor rotation"));

	FVector TraceDirection = CameraRotation.Vector();
	// 트레이스 시작/끝 위치 계산
	FVector TraceStart = WorldLocation;
	FVector TraceEnd = TraceStart + (TraceDirection * 150.f);

	// 라인 트레이스
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	// 라인 트레이스를 하여 무언가에 맞았는지를 나타냄
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Pawn, Params);
	

	// 맞은 액터가 어떤 컴포넌트를 가지고 있는지 검사
	if (bHit && Hit.GetActor()->FindComponentByClass<UPlayerStateComponent>())
	{
		Revive(Value);
	}
	else
	{
		MeleeAttack(Value);
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
	if (PlayerStateComponent->IsDead || PlayerStateComponent->IsKnocked || WeaponComponent->bIsWeaponAttacking || bIsReloading) return;
	if (CurrentAmmo > 0)
	{
		// 서버: 실제 발사 처리
		UE_LOG(LogTemp, Warning, TEXT("<Fire>"));
		WeaponComponent->Fire();
		CurrentAmmo --;
	}
}

void ALudens_PCharacter::Server_Reload_Implementation()
{
	HandleReload();
}

void ALudens_PCharacter::Reload(const FInputActionValue& Value)
{
	if (PlayerStateComponent->IsDead || PlayerStateComponent->IsKnocked) return;
	if (ReviveComponent && ReviveComponent->IsReviving())
	{
		ReviveComponent->CancelRevive(); // ← ReviveTimer 해제 + KnockedTimer 재개
	}

	if (GetLocalRole() < ROLE_Authority)
	{
		// 클라이언트 플레이어
		Server_Reload();
		return;
	}
	else HandleReload(); // 서버 플레이어
}

void ALudens_PCharacter::HandleReload()
{
	if (bIsReloading) return; // 이미 재장전 중이면 리턴
	
	bIsReloading = true;
	if (CurrentAmmo != MaxAmmo)
	{
		if (SavedAmmo <= 0)
		{
			return;
		}
		else if (SavedAmmo - (MaxAmmo - CurrentAmmo) <= 0)
		{
			CurrentAmmo += SavedAmmo;
			SavedAmmo = 0;
		}
		else
		{
			SavedAmmo -= (MaxAmmo-CurrentAmmo);
			CurrentAmmo = MaxAmmo;
		}
	}

	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ALudens_PCharacter::EndReload, ReloadTime, false);
}

void ALudens_PCharacter::EndReload()
{
	bIsReloading = false;
}

void ALudens_PCharacter::OnRep_SavedAmmo()
{
	// 젤루 흡수 or 재장전 시 변경되는 UI, 사운드 등
}

void ALudens_PCharacter::OnRep_CurrentAmmo()
{
	// 재장전 시 변경되는 UI, 사운드 등
}

int ALudens_PCharacter::GetCurrentAmmo() const // PlayerAttackComponent에서 현재 탄알 수 확인용
{
	return CurrentAmmo;
}

void ALudens_PCharacter::Server_Revive_Implementation()
{
	// 서버에서 소생 처리
	ReviveComponent->HandleRevive();
}

void ALudens_PCharacter::Revive(const FInputActionValue& Value)
{
	// 만약 클라이면 -> 서버에게 소생 요청
	if (GetLocalRole() < ROLE_Authority) Server_Revive();
	ReviveComponent->HandleRevive();
}

void ALudens_PCharacter::Absorb(const FInputActionValue& Value)
{
	// 클라의 경우 서버에서 로직 실행
	if (GetLocalRole() < ROLE_Authority) Server_Absorb(Value);
	
	if (PlayerStateComponent->IsDead || PlayerStateComponent->IsKnocked) return;
	if (ReviveComponent && ReviveComponent->IsReviving())
	{
		ReviveComponent->CancelRevive(); // ← ReviveTimer 해제 + KnockedTimer 재개
	}
	// 라인 트레이스를 통해 앞에 있는 대상이 무엇인지 판별
	// 화면 중심에서 월드 방향 구하기
	FVector WorldLocation = FirstPersonCameraComponent->GetComponentLocation();
	FRotator CameraRotation = GetActorRotation();
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		CameraRotation = PC->PlayerCameraManager->GetCameraRotation();
	}
	else UE_LOG(LogTemp, Warning, TEXT("❗ GetController() is null, fallback to actor rotation"));

	FVector TraceDirection = CameraRotation.Vector();
	// 트레이스 시작/끝 위치 계산
	FVector TraceStart = WorldLocation;
	FVector TraceEnd = TraceStart + (TraceDirection * 150.f);

	// 라인 트레이스
	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	// 라인 트레이스를 하여 무언가에 맞았는지를 나타냄
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Pawn, Params);

	if (bHit && Hit.GetActor())
	{
		if (Hit.GetActor()->FindComponentByClass<UJellooComponent>())
		{
			WeaponComponent->HandleAbsorb();
		}
	}
}

void ALudens_PCharacter::Server_Absorb_Implementation(const FInputActionValue& Value)
{
	Absorb(Value);
}

void ALudens_PCharacter::AbsorbComplete(const FInputActionValue& Value)
{
	
	if (GetLocalRole() < ROLE_Authority) Server_AbsorbComplete(Value);
	GetWorldTimerManager().SetTimer(
	   AbsorbCompleteTimerHandle,
	   FTimerDelegate::CreateUObject(WeaponComponent, &UTP_WeaponComponent::StopPerformAbsorb),
	   0.3f, 
	   false
   );
	UE_LOG(LogTemp, Log, TEXT("AbsorbComplete"));
}

void ALudens_PCharacter::Server_AbsorbComplete_Implementation(const FInputActionValue& Value)
{
	AbsorbComplete(Value);
}

void ALudens_PCharacter::MulticastControlDashEffect_Implementation(bool bActivate)
{
	if (DashNiagaraComponent)
	{
		if (bActivate)
		{
			// 모든 클라이언트에서 활성화 명령
			// 이미 활성화된 경우에도 Reset 옵션으로 재시작 (Set Active는 Reset 옵션을 포함합니다)
			DashNiagaraComponent->SetActive(true, true); 
            
			// 0.3초 후 DeactivateDashEffect 함수 호출하도록 타이머 설정
			GetWorld()->GetTimerManager().SetTimer(
				DashEffectTimerHandle,
				this,
				&ALudens_PCharacter::DeactivateDashEffect,
				0.3f, // 대시 이펙트 지속 시간
				false
			);
		}
		else
		{
			// 비활성화 명령 (0.3초 타이머에 의해 호출됨)
			DashNiagaraComponent->Deactivate();
            
			// 타이머 해제 (안전 장치)
			GetWorld()->GetTimerManager().ClearTimer(DashEffectTimerHandle);
		}
	}
}

void ALudens_PCharacter::DeactivateDashEffect()
{
	// MulticastControlDashEffect_Implementation(false)를 직접 호출
	// 서버에서 호출하면 Multicast로 동작하지 않으므로, 이펙트 비활성화는
	// 이미 클라이언트와 서버 모두에서 실행되고 있는 Deactivate 함수를 직접 호출하는 방식이 더 간단합니다.
	// 하지만 네트워크 안정성을 위해 Multicast 함수에 통합하는 것이 가장 좋습니다.
    
	// 여기서는 MulticastControlDashEffect_Implementation(false)를 대신 호출하는 로직을 가정합니다.
	// **다만, Multicast 함수의 Implementation을 직접 호출하는 것은 권장되지 않습니다.**
	// 가장 안전한 방법은 아래 Dash() 로직처럼, 
	// 서버에서만 SetTimer를 설정하고, 이 타이머 완료 시 Deactivate()를 호출하는 것입니다.

	// 🌟 안전한 방법: 클라이언트마다 타이머를 실행하고 로컬에서 비활성화
	if (DashNiagaraComponent)
	{
		DashNiagaraComponent->Deactivate();
	}
}

void ALudens_PCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALudens_PCharacter, JumpCount);
	DOREPLIFETIME(ALudens_PCharacter, CurrentDashCount);
	DOREPLIFETIME(ALudens_PCharacter, bCanDash);
	DOREPLIFETIME(ALudens_PCharacter, ReplicatedDashCooldownStartTime);
	DOREPLIFETIME(ALudens_PCharacter, SavedAmmo);
	DOREPLIFETIME(ALudens_PCharacter, CurrentAmmo);
}

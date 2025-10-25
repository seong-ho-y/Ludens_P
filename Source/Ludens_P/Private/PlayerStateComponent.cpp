// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerStateComponent.h"
#include "PlayerState_Real.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Ludens_P/Ludens_PGameMode.h"


// Sets default values for this component's properties
UPlayerStateComponent::UPlayerStateComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true); // 컴포넌트 복제를 활성화합니다.
}


// Called when the game starts
void UPlayerStateComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ACharacter>(GetOwner());
}

void UPlayerStateComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bPSRInitialized)
	{
		AActor* Owner = GetOwner();
		if (!Owner) return;

		APawn* Pawn = Cast<APawn>(Owner);
		if (!Pawn) return;

		PSR = Cast<APlayerState_Real>(Pawn->GetPlayerState());
		
		if (PSR)
		{
			CurrentHP = PSR->MaxHP;
			CurrentShield = PSR->MaxShield;
			MoveSpeed = PSR->MoveSpeed;
			CalculateMoveSpeed = MoveSpeed;

			if (Character)
			{
				// 이 컴포넌트가 부착된 캐릭터의 초기 이동 속도를 설정합니다.
				// OnRep_MoveSpeed가 초기값에 대해 호출될 수 있으므로 클라이언트에서도 필요합니다.
				Character->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
			}
			
			///

			if (GetOwnerRole() == ROLE_Authority && PSR)
			{
				PlayerColor = PSR->PlayerColor; 
			}

			///



			bPSRInitialized = true;  // 한 번만 실행되도록
		}
	}
	// 다른 Tick 로직에서도 PSR을 접근하는 경우
	if (!PSR) return;
    
	// 이후 안전하게 PSR 멤버 사용 가능
}


void UPlayerStateComponent::OnRep_PlayerColor()
{
	//
}


void UPlayerStateComponent::Multicast_PlayKnockedUI_Implementation()
{
	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (Char->IsLocallyControlled())
		{
			DamageVignetteOpacity = 1.0f; 
			// 💥 일반 데미지 타이머가 실행 중이더라도 멈춥니다.
			GetWorld()->GetTimerManager().ClearTimer(VignetteTimerHandle); 
		}
	}
}

void UPlayerStateComponent::UpdateVignetteOpacity()
{
	// 틱마다 Opacity를 감소시킵니다.
	DamageVignetteOpacity -= FApp::GetDeltaTime() / 1.0f; // 1.0f는 전체 지속 시간 (1초)
	DamageVignetteOpacity = FMath::Clamp(DamageVignetteOpacity, 0.0f, 1.0f);

	// Opacity가 0에 도달하면 타이머를 멈춥니다.
	if (DamageVignetteOpacity <= 0.0f)
	{
		GetWorld()->GetTimerManager().ClearTimer(VignetteTimerHandle);
	}
}

void UPlayerStateComponent::RevertMoveSpeed()
{
	MoveSpeed = PSR->MoveSpeed;
}

void UPlayerStateComponent::Multicast_PlayDamageUI_Implementation()
{
	// 이 함수는 서버와 모든 클라이언트에서 실행됩니다.
    
	// 폰을 현재 로컬 플레이어가 제어하는지 확인합니다.
	// 리스닝 서버 호스트와 모든 클라이언트의 플레이어 폰에서만 실행되어야 합니다.
	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (Char->IsLocallyControlled()) // 로컬 플레이어 폰일 때만 UI를 띄움
		{
			// 1. Opacity를 최대치(1.0)로 설정
			DamageVignetteOpacity = 1.0f; 

			// 2. 틱 없이 타이머로 Opacity를 부드럽게 감소시키는 함수 호출을 시작합니다.
			GetWorld()->GetTimerManager().SetTimer(
			   VignetteTimerHandle,
			   this,
			   &UPlayerStateComponent::UpdateVignetteOpacity,
			   0.016f, // 대략 60FPS의 델타 타임
			   true // 루프 설정
			);
		}
	}
}

void UPlayerStateComponent::TakeDamage(float Amount)
{
	if (IsDead || IsKnocked) return;
	if (IsAttacked) return; // 공격 받은 후 무적 상태
	if (CurrentHP <= 0.f) Knocked();

	bCanRegenShield = false;
	GetWorld()->GetTimerManager().ClearTimer(RegenShieldTimer);
	GetWorld()->GetTimerManager().ClearTimer(bCanRegenShieldTimer);

	// 이펙트를 재생하기 전에 현재 캐릭터가 Authority를 가졌는지 확인
	if (GetOwner()->HasAuthority())
	{
		Multicast_PlayDamageUI(); // 모든 클라이언트에게 UI 재생을 명령
	}
	else
	{
		Server_RequestDamageUI(); // 클라의 경우 서버에 UI 재생 요청
	}
	
	// 쉴드가 남아 있을 경우 쉴드가 먼저 데미지를 받음.
	if (CurrentShield > 0)
	{
		CurrentShield = FMath::Max(0.f, CurrentShield - Amount);
		UE_LOG(LogTemp, Warning, TEXT("Shield: %f"), CurrentShield);

	}
	else
	{
		CurrentHP = FMath::Max(0.f, CurrentHP - Amount);
		UE_LOG(LogTemp, Warning, TEXT("HP: %f"), CurrentHP);
	}

	GetWorld()->GetTimerManager().SetTimer(bCanRegenShieldTimer, this, &UPlayerStateComponent::EnableShieldRegen, 3.0f, false);
	
	if (CurrentHP <= 0.f) Knocked();
	else
	{
		// 공격 당한 상태로 설정하고 무적 타이머 시작
		IsAttacked = true;

		// 2초 후 무적 상태 해제
		const float InvincibilityDuration = 1.0f;
		GetWorld()->GetTimerManager().SetTimer(InvincibilityTimerHandle, this, &UPlayerStateComponent::ResetInvincibility, InvincibilityDuration, false);
	}
}

void UPlayerStateComponent::Server_RequestDamageUI_Implementation()
{
	Multicast_PlayDamageUI();
}

void UPlayerStateComponent::EnableShieldRegen()
{
	bCanRegenShield = true;
	GetWorld()->GetTimerManager().SetTimer(RegenShieldTimer, this, &UPlayerStateComponent::RegenShieldHandle, PSR->ShieldRegenSpeed, true);
}

void UPlayerStateComponent::RegenShieldHandle()
{
	if (!bCanRegenShield || CurrentShield >= MaxShield || IsKnocked || IsDead)
	{
		GetWorld()->GetTimerManager().ClearTimer(RegenShieldTimer);
		return;
	}
	CurrentShield += 1.f;
	//UE_LOG(LogTemp, Log, TEXT("Shield Regen: %f"), CurrentShield);
}

void UPlayerStateComponent::Knocked()
{
	// Knocked 상태 변경은 서버에서만 실행
	if (!GetOwner()->HasAuthority()) Server_Knocked();
	
	UE_LOG(LogTemp, Error, TEXT("Player Knocked!"));
	
	IsKnocked = true;
	MoveSpeed = KnockedMoveSpeed;
	if (Character)
		Character->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	// 서버는 UI를 띄워야 하므로 Multicast 호출
	// Multicast_PlayDamageUI(); // Knocked가 되었을 때도 UI를 띄웁니다.

	if (GetOwner()->HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("Player Knocked UI"));
		// 일반 데미지 UI와 동일한 Multicast를 호출하여 Opacity를 1.0으로 만듭니다.
		Multicast_PlayKnockedUI(); 
	}
	
	// OnRep 함수를 수동으로 호출하여 서버 자신에게도 로직을 적용합니다.
	// OnRep_Knocked();
	
	// 5초 뒤 Dead 함수를 호출하는 타이머를 설정
	GetWorld()->GetTimerManager().SetTimer(KnockedTimer, this, &UPlayerStateComponent::Dead, 15.0f, false);
}

void UPlayerStateComponent::Server_Knocked_Implementation()
{
	UE_LOG(LogTemp, Error, TEXT("Client Player Knocked!"));
	Knocked();
}

void UPlayerStateComponent::Dead()
{
	// Dead 상태 변경은 서버에서만 실행
	if (!GetOwner()->HasAuthority()) return;
	
	UE_LOG(LogTemp, Error, TEXT("Player Dead!"));
	IsKnocked = false;
	IsDead = true;
	MoveSpeed = DeadMoveSpeed;
	
	if (Character)
		Character->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	OnRep_Dead();

	// ▼ 한 명이라도 죽으면 게임오버 알림
	if (ALudens_PGameMode* GM = GetWorld()->GetAuthGameMode<ALudens_PGameMode>())
	{
		GM->NotifyAnyPlayerDead();
	}
}

void UPlayerStateComponent::ResetInvincibility() 
{
	IsAttacked = false;
}

void UPlayerStateComponent::OnRep_IsAttacked()
{
	// 클라이언트에서 피격 상태 변경 시 처리할 로직 (UI, 이펙트 등)
}

void UPlayerStateComponent::OnRep_Dead()
{
	// 클라이언트에서 죽은 상태 변경 시 처리할 로직 (UI, 이펙트 등)
	UE_LOG(LogTemp, Warning, TEXT("Client OnRep_Dead called."));
}

void UPlayerStateComponent::OnRep_Knocked()
{
	// 클라이언트에서 기절 상태 변경 시 처리할 로직 (UI, 이펙트 등)
	UE_LOG(LogTemp, Warning, TEXT("Client OnRep_Knocked called."));

	// 로컬 제어 확인
	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (Char->IsLocallyControlled())
		{
			if (IsKnocked) // 💥 기절 상태 시작 (True)
			{
				UE_LOG(LogTemp, Warning, TEXT("Player Knocked UI Play!!!!!"));
				// 1. Opacity를 최대 불투명도(1.0)로 강제 설정
				DamageVignetteOpacity = 1.0f; 
				// 혹시 실행 중일지 모르는 1초 데미지 타이머를 해제하고 중단
				GetWorld()->GetTimerManager().ClearTimer(VignetteTimerHandle);

				// 3. UI 바인딩이 즉시 업데이트되도록 강제 알림 (옵션, DOREPLIFETIME이 아니므로 강제하지 않아도 틱마다 업데이트됨)
				// 만약 Opacity가 UPROPERTY(Replicated)라면 여기서 OnRep_...를 수동 호출하지만, UI 변수이므로 생략합니다.
			}
			else // 기절 상태 해제 (False)
			{
				// 1. 비네트 효과를 즉시 제거합니다.
				DamageVignetteOpacity = 0.0f;

				// 2. 혹시 Opacity를 0.0으로 돌리는 타이머가 남아있을 수 있으므로 해제 (안전 장치)
				GetWorld()->GetTimerManager().ClearTimer(VignetteTimerHandle);
			}
		}
	}
}

void UPlayerStateComponent::OnRep_MoveSpeed()
{
	// 클라이언트에서 MoveSpeed 값이 복제되었을 때 호출됨.
	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Client OnRep_MoveSpeed called. New Speed: %f"), MoveSpeed);
		Character->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	}
}

void UPlayerStateComponent::UpdateMoveSpeed()
{
	if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
		{
			Move->MaxWalkSpeed = MoveSpeed;
		}
	}	
}

void UPlayerStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UPlayerStateComponent, MaxHP);
	DOREPLIFETIME(UPlayerStateComponent, CurrentHP);
	DOREPLIFETIME(UPlayerStateComponent, MaxShield);
	DOREPLIFETIME(UPlayerStateComponent, CurrentShield);
	DOREPLIFETIME(UPlayerStateComponent, IsAttacked);
	DOREPLIFETIME(UPlayerStateComponent, IsKnocked);
	DOREPLIFETIME(UPlayerStateComponent, IsDead);
	DOREPLIFETIME(UPlayerStateComponent, MoveSpeed);

	// 상성 색 복제 추가

	DOREPLIFETIME(UPlayerStateComponent, PlayerColor);
	DOREPLIFETIME(UPlayerStateComponent, bCanRegenShield); // 경고 해소
}

static FORCEINLINE void OpApplyF(float& S, ERewardOpType Op, float V)
{
	if (Op == ERewardOpType::Multiply) S *= V; else S += V;
}

void UPlayerStateComponent::ApplyMaxHP(ERewardOpType Op, float V, ECurrentHPPolicy Policy)
{
	AActor* Owner = GetOwner();
	if (!(Owner && Owner->HasAuthority())) return;

	const float OldMax = MaxHP;
	const float OldCur = CurrentHP;

	OpApplyF(MaxHP, Op, V);
	MaxHP = FMath::Max(1.f, MaxHP);

	switch (Policy)
	{
	case ECurrentHPPolicy::KeepRatio: {
		const float Ratio = (OldMax > 0.f) ? (OldCur / OldMax) : 1.f;
		CurrentHP = FMath::Clamp(Ratio * MaxHP, 0.f, MaxHP);
	} break;
	case ECurrentHPPolicy::KeepCurrent: {
		CurrentHP = FMath::Clamp(CurrentHP, 0.f, MaxHP);
	} break;
	case ECurrentHPPolicy::HealToFull: {
		CurrentHP = MaxHP;
	} break;
	default: break;
	}

	// ★ PSR 백업 동기화(로그/위젯에서 PSR을 봐도 일치)
	if (PSR)
	{
		PSR->MaxHP = MaxHP;
		PSR->ForceNetUpdate();
	}

	// TODO: HP 변경 이벤트/HUD 알림 필요 시 브로드캐스트
}

void UPlayerStateComponent::ApplyMaxShield(ERewardOpType Op, float V, ECurrentHPPolicy Policy)
{
	AActor* Owner = GetOwner();
	if (!(Owner && Owner->HasAuthority())) return;

	const float OldMax = MaxShield;
	const float OldCur = CurrentShield;

	OpApplyF(MaxShield, Op, V);
	MaxShield = FMath::Max(0.f, MaxShield);

	switch (Policy)
	{
	case ECurrentHPPolicy::KeepRatio: {
		const float Ratio = (OldMax > 0.f) ? (OldCur / OldMax) : 1.f;
		CurrentShield = FMath::Clamp(Ratio * MaxShield, 0.f, MaxShield);
	} break;
	case ECurrentHPPolicy::KeepCurrent: {
		CurrentShield = FMath::Clamp(CurrentShield, 0.f, MaxShield);
	} break;
	case ECurrentHPPolicy::HealToFull: {
		CurrentShield = MaxShield;
	} break;
	default: break;
	}

	// ★ PSR 백업 동기화
	if (PSR)
	{
		PSR->MaxShield = MaxShield;
		PSR->ForceNetUpdate();
	}
}

void UPlayerStateComponent::ApplyShieldRegenSpeed(ERewardOpType Op, float V)
{
	AActor* Owner = GetOwner();
	if (!(Owner && Owner->HasAuthority())) return;

	OpApplyF(PSR->ShieldRegenSpeed, Op, V);
	PSR->ShieldRegenSpeed = FMath::Clamp(PSR->ShieldRegenSpeed, 0.f, 10000.f);

	// 현재 타이머/틱 로직이 재생률을 사용한다면, 다음 틱부터 반영되도록 여기서 리셋/갱신
	// ResetShieldRegenTimer();
}

void UPlayerStateComponent::SyncMoveSpeedFromPSR(APlayerState_Real* PS_R)
{
	if (!PSR) return;
	MoveSpeed = PSR->MoveSpeed;

	// 프로젝트 구조상 CalculateMoveSpeed가 소스로 쓰이면 같이 맞춰줍니다.
	CalculateMoveSpeed = MoveSpeed;

	UpdateMoveSpeed();
}
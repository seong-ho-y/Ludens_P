// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ludens_PCharacter.h"

#include "EnemyBase.h"
#include "EngineUtils.h"
#include "Ludens_PProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Util/ColorConstants.h"

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

		//Dash
		//Fire 입력
		PlayerInputComponent->BindAction("Fire", IE_Pressed, Weapon, &UTP_WeaponComponent::Fire);

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

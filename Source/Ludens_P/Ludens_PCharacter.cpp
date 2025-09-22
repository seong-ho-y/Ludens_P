// Copyright Epic Games, Inc. All Rights Reserved.

#include "Ludens_PCharacter.h"
#include "Ludens_PProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
<<<<<<< Updated upstream
=======
#include "LudensAppearanceData.h" 
#include "LobbyPlayerState.h"
#include "Materials/MaterialInterface.h"
#include "LobbyTypes.h" 



>>>>>>> Stashed changes

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

<<<<<<< Updated upstream
=======
	// 3인칭 본체(Mesh3P = GetMesh()) 가시성/충돌/트랜스폼 세팅 

	if (USkeletalMeshComponent* Body = GetMesh())
	{
		Body->SetOnlyOwnerSee(false);                      // 타인에게 보이게
		Body->SetOwnerNoSee(true);                         // 본인에겐 숨김
		Body->bCastHiddenShadow = false;                
		Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// 기본 UE 마네킨 기준: 발이 바닥에 맞도록 대략값 (메쉬에 따라 Z만 미세 조정)
		Body->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		Body->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		Body->SetRelativeScale3D(FVector(1.f));
	}

	// ===== [추가] 1인칭 팔(Mesh1P) 충돌 보정 =====
	if (Mesh1P)
	{
		Mesh1P->SetOwnerNoSee(false);
		Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
>>>>>>> Stashed changes
}

void ALudens_PCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
<<<<<<< Updated upstream
=======

	CacheMesh3P();  // 

	// 전신/팔 메쉬가 제대로 들어왔는지 확인
	UE_LOG(LogTemplateCharacter, Display, TEXT("[Char] 3P=%s 1P=%s"),
		*GetNameSafe(GetMesh() ? GetMesh()->GetSkeletalMeshAsset() : nullptr),
		*GetNameSafe(Mesh1P ? Mesh1P->GetSkeletalMeshAsset() : nullptr));
>>>>>>> Stashed changes
}

//////////////////////////////////////////////////////////////////////////// Input

void ALudens_PCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALudens_PCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALudens_PCharacter::Look);
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
<<<<<<< Updated upstream
}
=======
}

void ALudens_PCharacter::CacheMesh3P()
{
	if (!Mesh3P) Mesh3P = GetMesh();
}

void ALudens_PCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	BindToLobbyPS();  // 서버측: 소유 직후

	UE_LOG(LogTemplateCharacter, Display, TEXT("[Char] PossessedBy/OnRep_PlayerState -> BindToLobbyPS"));

}

void ALudens_PCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	BindToLobbyPS();  // 클라측: PS 복제 도착

	UE_LOG(LogTemplateCharacter, Display, TEXT("[Char] PossessedBy/OnRep_PlayerState -> BindToLobbyPS"));

}

void ALudens_PCharacter::BindToLobbyPS()
{
	if (bPSBound) return;

	if (ALobbyPlayerState* LPS = GetPlayerState<ALobbyPlayerState>())
	{
		LPS->OnAnyLobbyFieldChanged.AddDynamic(this, &ALudens_PCharacter::OnLobbyPSChanged);
		bPSBound = true;

		// 초기 1회 적용
		ApplyCosmeticsFromPS();
	}
}

void ALudens_PCharacter::OnLobbyPSChanged()
{
	ApplyCosmeticsFromPS();
}

void ALudens_PCharacter::ApplyCosmeticsFromPS()
{

	UE_LOG(LogTemplateCharacter, Display, TEXT("[Char] ApplyCosmeticsFromPS start"));
    ALobbyPlayerState* LPS = GetPlayerState<ALobbyPlayerState>();
	UE_LOG(LogTemplateCharacter, Display, TEXT("[Char] LPS=%s  AppearanceDB=%s"),
		*GetNameSafe(LPS), *GetNameSafe(AppearanceDB));
    if (!LPS || !AppearanceDB) return;

    CacheMesh3P();
    USkeletalMeshComponent* Body = Mesh3P;
    if (!Body) return;

    // Ready면 Selected, 아니면 Preview. 미선택(None)도 Red 취급
    ELobbyColor Chosen = LPS->bReady ? LPS->SelectedColor : LPS->PreviewColor;
	UE_LOG(LogTemplateCharacter, Display, TEXT("[Char] AppearId=%d  Ready=%d  Selected=%d  Preview=%d  Chosen=%d"),
		LPS->AppearanceId, (int)LPS->bReady, (int)LPS->SelectedColor, (int)LPS->PreviewColor, (int)Chosen);
    if (Chosen == ELobbyColor::None) Chosen = ELobbyColor::Red;

    const int32 Ap = FMath::Max(0, LPS->AppearanceId);
    AppearanceDB->ApplyTo(Body, Ap, Chosen);

    // 1P 팔은 아직 적용하지 않음 (나중에 에셋 들어오면 필요 시 동일 호출)



}


>>>>>>> Stashed changes

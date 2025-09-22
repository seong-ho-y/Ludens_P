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
	// 3��Ī ��ü(Mesh3P = GetMesh()) ���ü�/�浹/Ʈ������ ���� 

	if (USkeletalMeshComponent* Body = GetMesh())
	{
		Body->SetOnlyOwnerSee(false);                      // Ÿ�ο��� ���̰�
		Body->SetOwnerNoSee(true);                         // ���ο��� ����
		Body->bCastHiddenShadow = false;                
		Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// �⺻ UE ����Ų ����: ���� �ٴڿ� �µ��� �뷫�� (�޽��� ���� Z�� �̼� ����)
		Body->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		Body->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		Body->SetRelativeScale3D(FVector(1.f));
	}

	// ===== [�߰�] 1��Ī ��(Mesh1P) �浹 ���� =====
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

	// ����/�� �޽��� ����� ���Դ��� Ȯ��
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
	BindToLobbyPS();  // ������: ���� ����

	UE_LOG(LogTemplateCharacter, Display, TEXT("[Char] PossessedBy/OnRep_PlayerState -> BindToLobbyPS"));

}

void ALudens_PCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	BindToLobbyPS();  // Ŭ����: PS ���� ����

	UE_LOG(LogTemplateCharacter, Display, TEXT("[Char] PossessedBy/OnRep_PlayerState -> BindToLobbyPS"));

}

void ALudens_PCharacter::BindToLobbyPS()
{
	if (bPSBound) return;

	if (ALobbyPlayerState* LPS = GetPlayerState<ALobbyPlayerState>())
	{
		LPS->OnAnyLobbyFieldChanged.AddDynamic(this, &ALudens_PCharacter::OnLobbyPSChanged);
		bPSBound = true;

		// �ʱ� 1ȸ ����
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

    // Ready�� Selected, �ƴϸ� Preview. �̼���(None)�� Red ���
    ELobbyColor Chosen = LPS->bReady ? LPS->SelectedColor : LPS->PreviewColor;
	UE_LOG(LogTemplateCharacter, Display, TEXT("[Char] AppearId=%d  Ready=%d  Selected=%d  Preview=%d  Chosen=%d"),
		LPS->AppearanceId, (int)LPS->bReady, (int)LPS->SelectedColor, (int)LPS->PreviewColor, (int)Chosen);
    if (Chosen == ELobbyColor::None) Chosen = ELobbyColor::Red;

    const int32 Ap = FMath::Max(0, LPS->AppearanceId);
    AppearanceDB->ApplyTo(Body, Ap, Chosen);

    // 1P ���� ���� �������� ���� (���߿� ���� ������ �ʿ� �� ���� ȣ��)



}


>>>>>>> Stashed changes

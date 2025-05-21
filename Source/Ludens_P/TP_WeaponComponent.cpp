// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"

#include "EnemyBase.h"
#include "EnemyPoolManager.h"
#include "Ludens_PCharacter.h"
#include "Ludens_PProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Projects.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

// Sets default values for this component's properties
// 무기 종류와 Fire 메서드, 탄약, 재장전 등 무기와 관련된 메서드들
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}
void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!Character)
	{
		Character = Cast<ALudens_PCharacter>(GetOwner());
		if (!Character)
		{
			UE_LOG(LogTemp, Error, TEXT("WeaponComponent could not find owning character!"));
		}
	}
}

void UTP_WeaponComponent::Fire()
{
	if (!Character)
	{
		Character = Cast<ALudens_PCharacter>(GetOwner());
		if (!Character)
		{
			UE_LOG(LogTemp, Error, TEXT("❌ Character is null"));
			return;
		}
	}

	FRotator SpawnRotation = Character->GetActorRotation();

	if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
	{
		SpawnRotation = PC->PlayerCameraManager->GetCameraRotation();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("❗ GetController() is null, fallback to actor rotation"));
	}

		UE_LOG(LogTemp, Log, TEXT("Fire"));
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			UE_LOG(LogTemp, Log, TEXT("FireFIRE"));
			// 소환할 프로젝타일의 위치 조정하기 (카메라 Vector 가져와서 위치조정하면 될듯)
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);
	
			//Set Spawn Collision Handling Override
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner(); // 캐릭터 등
			SpawnParams.Instigator = Cast<APawn>(GetOwner()); // 발사체를 쏜 주체
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
	
			// Spawn the projectile at the muzzle
			ALudens_PProjectile* Projectile = World->SpawnActor<ALudens_PProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
		}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("ProjectileCLass is nullptr"));
	}
	// -----여긱까지가 소환 로직-------
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

/*bool UTP_WeaponComponent::AttachWeapon(ALudens_PCharacter* TargetCharacter) //일단 이거는 필요가 없음
{
	Character = TargetCharacter; //얘만 선언해주면 됨

	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UTP_WeaponComponent>())
	{
		return false;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// add the weapon as an instance component to the character
	Character->AddInstanceComponent(this);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}

	return true;
}
*/

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) //필요 없을거같은데
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "ShieldComponent.h"

#include "EEnemyColor.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UShieldComponent::UShieldComponent()
{
	SetIsReplicatedByDefault(true);
}

void UShieldComponent::TakeShieldDamage(float DamageAmount, EEnemyColor DamageColor)
{
	//서버에서만 처리
	if (!GetOwner()->HasAuthority()) return;
	// 클라이언트에 쉴드 상태 변경을 알리기


	UE_LOG(LogTemp, Log, TEXT("ShieldComponent::TakeShieldDamage"));

	// ✨ 디버그 로그 추가 (1단계)
	UE_LOG(LogTemp, Warning, TEXT("Checking shields... ActiveShields Count: %d"), ActiveShields.Num());

	
	for (FShieldLayer& Shield: ActiveShields)
	{
		// ✨ 디버그 로그 추가 (2단계) - 각 쉴드의 상태와 들어온 데미지 색상을 비교
		// UEnum::GetValueAsString으로 Enum 값을 문자열로 볼 수 있어 편리합니다.
		UE_LOG(LogTemp, Log, TEXT("Checking Shield -> ShieldColor: %s, Health: %.1f | Incoming DamageColor: %s"), 
			*UEnum::GetValueAsString(Shield.ShieldColor), 
			Shield.CurrentHealth, 
			*UEnum::GetValueAsString(DamageColor));
		if (Shield.ShieldColor == DamageColor && Shield.CurrentHealth > 0)
		{
			float AbsorbedDamage = FMath::Min(Shield.CurrentHealth, DamageAmount);
			Shield.CurrentHealth -= AbsorbedDamage;

			UE_LOG(LogTemp, Log, TEXT("SUCCESS: Shield took %.1f damage. Current Health: %.1f"), 
		  AbsorbedDamage, Shield.CurrentHealth);
			OnRep_ActiveShields();
		}
	}
}

bool UShieldComponent::AreAllShieldsBroken() const
{
	for (const FShieldLayer& Shield : ActiveShields)
	{
		if (Shield.CurrentHealth > 0)
		{
			return false;
		}
	}
	return true;
}

void UShieldComponent::InitializeShields(EEnemyColor EnemyColorType)
{
	//역시 서버에서만 해야된다
	if (!GetOwner()->HasAuthority()) return;
	//UE_LOG(LogTemp, Log, TEXT("Shield activated"));
	ActiveShields.Empty(); //기존 쉴드 목록 초기화 Array 클래스의 Empty 메서드

	//쉴드를 추가해주기
	auto AddShield = [&](EEnemyColor Color)
	{
		FShieldLayer NewShield;
		NewShield.ShieldColor = Color;
		NewShield.MaxHealth = DefaultShieldHealth;
		NewShield.CurrentHealth = DefaultShieldHealth;
		ActiveShields.Add(NewShield);
	};
	//조건에 따라 AddShield호출
	switch (EnemyColorType)
	{
		case EEnemyColor::Red:
			AddShield(EEnemyColor::Red);
			break;
		case EEnemyColor::Green:
			AddShield(EEnemyColor::Green);
			break;
	case EEnemyColor::Blue:
		AddShield(EEnemyColor::Blue);
		break;
	case EEnemyColor::Yellow:
		AddShield(EEnemyColor::Red);
		AddShield(EEnemyColor::Green);
		break;
	case EEnemyColor::Magenta:
		AddShield(EEnemyColor::Red);
		AddShield(EEnemyColor::Blue);
		break;
	case EEnemyColor::Cyan:
		AddShield(EEnemyColor::Green);
		AddShield(EEnemyColor::Blue);
		break;
	case EEnemyColor::Black:
		AddShield(EEnemyColor::Red);
		AddShield(EEnemyColor::Green);
		AddShield(EEnemyColor::Blue);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("!!Invalid Color!!"));
		break;
	}

	//서버에서 쉴드 목록 변경했으니 클라에 변경사항 알리기
	OnRep_ActiveShields();
	
}
void UShieldComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// ActiveShields 변수를 모든 클라이언트에 복제하도록 등록한다
	DOREPLIFETIME(UShieldComponent, ActiveShields);
}
void UShieldComponent::OnRep_ActiveShields()
{
	// 쉴드 배열 정보가 클라이언트에서 업데이트되었을 때 호출됩니다.
	// (서버에서는 수동으로 호출해야 함)
	// 이 델리게이트를 UI 위젯 등에서 바인딩하여 쉴드 UI를 업데이트할 수 있습니다.
	OnShieldsUpdated.Broadcast();
}
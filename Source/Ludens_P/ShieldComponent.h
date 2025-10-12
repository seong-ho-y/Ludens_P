// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShieldComponent.generated.h"

enum class EEnemyColor : uint8;

USTRUCT(BlueprintType)
struct FShieldLayer
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Shield")
	EEnemyColor ShieldColor;

	UPROPERTY(EditDefaultsOnly, Category = "Shield")
	float MaxHealth = 10.f; //최대 쉴드 체력
	UPROPERTY(EditDefaultsOnly, Category = "Shield")
	float CurrentHealth = 10.f; //현재 쉴드 체력
};

//델리게이트를 사용할거임
//쉴드 정보가 변했을 때에 UI 변화, 서버와 클라가 공유해야되기에 멀티캐스트 델리게이트 생성
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShieldsUpdated);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UShieldComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShieldComponent();

	void TakeShieldDamage(float DamageAmount, EEnemyColor DamageColor);

	void InitializeShields(EEnemyColor EnemyColorType);

	UFUNCTION(BlueprintPure, Category = "Shield")
	bool AreAllShieldsBroken() const;

	//이 함수는 블루프린트에서 현재 쉴드를 가져오는 함수
	//UI와 연동하기 위해 필요
	UFUNCTION(BlueprintPure, Category = "Shield")
	const TArray<FShieldLayer>& GetActiveShields() const { return ActiveShields; }

	//쉴드 정보 업데이트 시 UI에 알리기 위한 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Shield")
	FOnShieldsUpdated OnShieldsUpdated;
	

protected:
	//네트워크 복제에 필요한 코드 - UPROPERTY(Replicated)를 쓰기 위해
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// 이 컴포넌트가 가진 모든 쉴드 레이어
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_ActiveShields, Category="Shield")
	TArray<FShieldLayer> ActiveShields;

public:
	// 각 쉴드의 기본 체력. 블루프린트에서 수정 가능
	UPROPERTY(EditDefaultsOnly, Category="Shield Config")
	float DefaultShieldHealth = 10.f;

	// ActiveShields 배열이 클라이언트에서 복제 완료되면 호출됨
	UFUNCTION()
	void OnRep_ActiveShields();
		
};

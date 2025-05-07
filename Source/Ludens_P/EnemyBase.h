#pragma once

#include "CoreMinimal.h"
#include "CreatureCombatComponent.h"
#include "WalkerAIComponent.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UCLASS()
class LUDENS_P_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

	
	virtual void BeginPlay() override;
	// === 전투 컴포넌트 ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Component")
	UCreatureCombatComponent* Combat;

	// === 워커 AI 컴포넌트 ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Component")
	UWalkerAIComponent* WalkerAI;

	// (선택) 시각 효과용 컴포넌트
	// UPROPERTY(...)
	// UEnemyVisualComponent* Visual;

public:
	void SetActive(bool bNewActive);
	bool IsActive() const;
	
	virtual void Tick(float DeltaTime) override;
	
private:
	bool bActive = false;

};

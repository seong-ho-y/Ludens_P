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


public:
	virtual void SetActive(bool bNewActive);
	bool IsActive() const {return bActive;}	
	
	virtual void Tick(float DeltaTime) override;
	
private:
	bool bActive = false;

};

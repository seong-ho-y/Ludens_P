#pragma once

#include "CoreMinimal.h"
#include "CreatureCombatComponent.h"
#include "WalkerAIComponent.h"
#include "StealthComponent.h"
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStealthComponent* StealthComponent;

public:
	virtual void SetActive(bool bNewActive);
	void PostNetInit();
	void OnPostReplicationInit();
	bool IsActive() const;

	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetActive(bool bNewActive);


private:
	bool bActive = false;

};

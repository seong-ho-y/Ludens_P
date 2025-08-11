#pragma once

#include "CoreMinimal.h"
#include "CreatureCombatComponent.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

class UEnemyDescriptor;

UCLASS(Abstract)
class AEnemyBase : public ACharacter
{
	GENERATED_BODY()
public:
	AEnemyBase();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UCreatureCombatComponent* CCC;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
							 AController* EventInstigator, AActor* DamageCauser) override;

	UPROPERTY(EditDefaultsOnly, Category="Config")
	UEnemyDescriptor* Descriptor = nullptr;
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDied();
};
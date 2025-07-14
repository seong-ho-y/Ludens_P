#pragma once

#include "CoreMinimal.h"
#include "CreatureCombatComponent.h"
#include "WalkerAIComponent.h"
#include "TP_WeaponComponent.h"
#include "StealthComponent.h"
#include "GameFramework/Character.h"
#include "EnemyColorEnum.h"
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

	virtual void SetActive(bool bNewActive);
	void PostNetInit();
	void OnPostReplicationInit();
	bool IsActive() const;

	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetActive(bool bNewActive);

	//원거리 형 적들이 공격을 할 수 있게
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	UTP_WeaponComponent* ShooterWeaponComponent;

	UPROPERTY(BlueprintCallable, Category = "Enemy Color")
	void SetEnemyMeshMaterial(EEnemyColor NewColor);
	
protected:
	UFUNCTION(EditAnywhere, BlueprintReadWrite, Category = "Enemy Color")
	TMap<EEnemyColor, UMaterialInstance*> EnemyMaterials;

	UPROPERTY(EditAnywhere, Category = "Enemy Color")
	int32 MaterialSlotIndex;
	
private:
	bool bActive = false;

};

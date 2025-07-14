#pragma once

#include "CoreMinimal.h"
#include "CreatureCombatComponent.h"
#include "WalkerAIComponent.h"
#include "TP_WeaponComponent.h"
#include "StealthComponent.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"



// 적의 색을 저장하는 enum
UENUM(Meta = (Bitflags, DisplayName = "Enemy Colors"))
enum class EEnemyColor : uint8
{
	White = 0 UMETA(DisplayName = "White"),
	Blue = 1 << 0 UMETA(DisplayName = "Blue"), // 0000 0001
	Green = 1 << 1 UMETA(DisplayName = "Green"), // 0000 0010
	Red = 1 << 2 UMETA(DisplayName = "Red"), // 0000 0100
	Cyan = Blue | Green UMETA(DisplayName = "Cyan"), // 0000 0011
	Magenta = Blue | Red UMETA(DisplayName = "Magenta"), // 0000 0101
	Yellow = Red | Green UMETA(DisplayName = "Yellow"), // 0000 0110
	Black = Blue | Green | Red UMETA(DisplayName = "Black"), // 0000 0111
};
ENUM_CLASS_FLAGS(EEnemyColor) // 비트 연산자 오버로딩을 위한 매크로

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

	//색 관련해서
	UPROPERTY(BlueprintCallable, Category = "Enemy Color")
	void SetEnemyMeshMaterial(EEnemyColor NewColor);

	
protected:
	UFUNCTION(EditAnywhere, BlueprintReadWrite, Category = "Enemy Color")
	TMap<EEnemyColor, UMaterialInterface*> EnemyMaterials;

	UPROPERTY(EditAnywhere, Category = "Enemy Color")
	int32 MaterialSlotIndex;
	
private:
	bool bActive = false;

};

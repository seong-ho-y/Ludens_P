
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "EnemyPMComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LUDENS_P_API UEnemyPMComponent : public UProjectileMovementComponent
{
	GENERATED_BODY()

public:
	// 에디터에서 수정할 수 있는 전방 가속도 변수를 추가합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float ForwardAcceleration = 100.0f;

protected:
	// 부모 클래스의 ComputeAcceleration 함수를 재정의(override)합니다.
	virtual FVector ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const override;
};
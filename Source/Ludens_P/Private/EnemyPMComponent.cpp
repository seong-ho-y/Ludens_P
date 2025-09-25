// AccelProjectileMovementComponent.cpp

#include "EnemyPMComponent.h"

FVector UEnemyPMComponent::ComputeAcceleration(const FVector& InVelocity, float DeltaTime) const
{
	// 1. 먼저 부모 클래스의 원래 가속도(중력 등)를 계산합니다.
	FVector Acceleration = Super::ComputeAcceleration(InVelocity, DeltaTime);

	// 2. 우리가 설정한 전방 가속도를 추가로 더해줍니다.
	// UpdatedComponent는 이 컴포넌트가 움직이는 대상을 가리킵니다 (보통 프로젝타일의 루트).
	if (UpdatedComponent)
	{
		Acceleration += UpdatedComponent->GetForwardVector() * Acceleration;
	}

	// 3. 최종 계산된 가속도를 반환합니다.
	return Acceleration;
}
// StealthComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"   // UActorComponent
#include "GameFramework/Actor.h"         // AActor
#include "StealthComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LUDENS_P_API UStealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStealthComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ActivateStealth();     // 투명화 시작
	void DeactivateStealth();   // 투명화 해제

private:
	bool bIsStealthed = false;
	float StealthCheckInterval = 0.2f;
	float TimeSinceLastCheck = 0.0f;

	UPROPERTY(EditAnywhere, Category="Stealth")
	float RevealDistance = 600.f;

	void SetVisibility(bool bVisible);

	AActor* FindNearestPlayer();
};

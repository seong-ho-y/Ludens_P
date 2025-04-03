#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

UCLASS()
class LUDENS_P_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// 체력 변수 (기본값)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy")
	float HP = 100.f;
};

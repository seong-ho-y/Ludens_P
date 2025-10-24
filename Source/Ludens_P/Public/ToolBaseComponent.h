// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToolInterface.h"
#include "Components/ActorComponent.h"
#include "ToolBaseComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UToolBaseComponent : public UActorComponent, public IToolInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UToolBaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Tool|Sound")
	USoundBase* UseSound; // 공통 또는 개별 사운드 설정 가능

	UFUNCTION(BlueprintCallable, Category = "Tool|Sound")
	virtual void PlayToolSound(APawn* InstigatorPawn);

public:	
	// Called every frame
	// 쿨타임 관련
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tool|Cooldown")
	float CooldownTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tool|Cooldown")
	bool bIsOnCooldown;

	FTimerHandle CooldownTimerHandle;

	UFUNCTION()
	virtual void StartCooldown();

	UFUNCTION()
	virtual void EndCooldown();

public:
	// ToolInterface 구현
	virtual bool CanUseTool_Implementation() const override;
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	// 각 도구가 override할 핵심 동작
	virtual void PerformToolAction(APawn* InstigatorPawn);		
};

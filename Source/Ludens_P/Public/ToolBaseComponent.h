// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToolInterface.h"
#include "Components/ActorComponent.h"
#include "ToolBaseComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FToolCooldownUpdated, float, CooldownPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FToolCooldownEnded);


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

	UPROPERTY(ReplicatedUsing = OnRep_CooldownPercent, BlueprintReadOnly, Category = "Tool|Cooldown")
	float CooldownPercent; // 0~1

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Tool|Cooldown")
	bool bIsOnCooldown;  // ← 반드시 Replicated 표시 필요!!

	UFUNCTION()
	void OnRep_CooldownPercent();

	UFUNCTION(Server, Reliable)
	void Server_StartCooldown();
	

	float ElapsedTime;

public:	
	// Called every frame
	// 쿨타임 관련
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tool|Cooldown")
	float CooldownTime;
	float CooldownElapsed;
	FTimerHandle CooldownTimerHandle;


	UPROPERTY(BlueprintAssignable, Category="Tool|Event")
	FToolCooldownUpdated OnCooldownUpdated;

	UPROPERTY(BlueprintAssignable, Category="Tool|Event")
	FToolCooldownEnded OnCooldownEnded;
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

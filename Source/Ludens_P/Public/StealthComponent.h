// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LUDENS_P_API UStealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStealthComponent();

	UFUNCTION(BlueprintCallable, Category = "Stealth")
	void EnterStealth();

	UFUNCTION(BlueprintCallable, Category = "Stealth")
	void ExitStealth();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TArray<class UMaterialInstanceDynamic*> DynamicMaterials;

	UPROPERTY(EditDefaultsOnly, Category = "Stealth | Parameters")
	FName OpacityParamName = TEXT("OpacityAmount"); //이름으로 설정해줄거임

	UPROPERTY(EditDefaultsOnly, Category = "Stealth | Parameters")
	float StealthOpacity = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Stealth | Parameters")
	float ChangeSpeed = 3.0f;

	float CurrentOpacity;
	float TargetOpacity;
};

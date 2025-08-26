// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RewardData.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class LUDENS_P_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 에디터에서 데이터 테이블을 할당할 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	UDataTable* RewardDataTable;

	// 마스터 보상 목록을 가져오는 함수. 이 함수만 외부에 공개됩니다.
	const TArray<FRewardData>& GetRewardMasterList();

private:
	// 로드된 보상 데이터를 캐싱(저장)해 둘 배열
	UPROPERTY()
	TArray<FRewardData> CachedRewardMasterList;

	// 이미 로드했는지 확인하는 플래그
	bool bIsRewardListLoaded = false;
};

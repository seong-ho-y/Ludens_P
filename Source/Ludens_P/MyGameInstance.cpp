#include "MyGameInstance.h"
#include "Engine/DataTable.h"

const TArray<FRewardData>& UMyGameInstance::GetRewardMasterList()
{
	// 아직 로드된 적이 없다면 데이터 테이블을 로드합니다. (Lazy Loading)
	if (!bIsRewardListLoaded)
	{
		if (RewardDataTable)
		{
			TArray<FName> RowNames = RewardDataTable->GetRowNames();
			for (const FName& RowName : RowNames)
			{
				FRewardData* RowData = RewardDataTable->FindRow<FRewardData>(RowName, TEXT(""));
				if (RowData)
				{
					CachedRewardMasterList.Add(*RowData);
				}
			}
		}
		bIsRewardListLoaded = true;
		UE_LOG(LogTemp, Warning, TEXT("=== GameInstance: Reward Master List Loaded ONCE. Count: %d ==="), CachedRewardMasterList.Num());
	}

	// 로드된 데이터를 반환합니다.
	return CachedRewardMasterList;
}
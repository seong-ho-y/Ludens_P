// BTT_EngageStealth.cpp

#include "BTT_EngageStealth.h"
#include "AIController.h"
#include "GameFramework/Character.h" // ACharacter 헤더 추가
#include "Components/SkeletalMeshComponent.h" // USkeletalMeshComponent 헤더 추가
#include "Ludens_P/EnemyBase.h"
#include "Materials/MaterialInstanceDynamic.h" // UMaterialInstanceDynamic 헤더 추가

UBTT_EngageStealth::UBTT_EngageStealth()
	{
		NodeName = TEXT("Engage Stealth (Direct)");

	}

EBTNodeResult::Type UBTT_EngageStealth::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AICon = OwnerComp.GetAIOwner();
	if (!AICon) return EBTNodeResult::Failed;

	AEnemyBase* MyEnemy = Cast<AEnemyBase>(AICon->GetPawn());
	if (MyEnemy)
	{
		// ❗ 이제 BTT는 MID를 직접 제어하는 대신, 복제될 변수의 값만 변경합니다.
		// MyEnemy->StealthAmount = 1.0f; // 직접 접근 (변수가 public일 경우)
		MyEnemy->SetStealthAmount(0.0f); // Setter 함수를 만들어서 사용하는 것을 추천

		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}

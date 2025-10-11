// BTT_EngageStealth.cpp

#include "BTT_EngageStealth.h"
#include "AIController.h"
#include "GameFramework/Character.h" // ACharacter 헤더 추가
#include "Components/SkeletalMeshComponent.h" // USkeletalMeshComponent 헤더 추가
#include "Materials/MaterialInstanceDynamic.h" // UMaterialInstanceDynamic 헤더 추가

UBTT_EngageStealth::UBTT_EngageStealth()
	{
		NodeName = TEXT("Engage Stealth (Direct)");

	}

EBTNodeResult::Type UBTT_EngageStealth::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
	{
		// 1. AI 컨트롤러 및 폰 가져오기
		AAIController* AICon = OwnerComp.GetAIOwner();
		if (!AICon) return EBTNodeResult::Failed;

		// 2. 폰을 실제 캐릭터 클래스로 캐스팅하여 메시 컴포넌트 가져오기
		//    AYourAICharacter는 실제 사용하시는 AI 캐릭터 클래스 이름으로 바꿔주세요.
		ACharacter* MyCharacter = Cast<ACharacter>(AICon->GetPawn());
		if (!MyCharacter) return EBTNodeResult::Failed;

		USkeletalMeshComponent* MyMesh = MyCharacter->GetMesh();
		if (!MyMesh) return EBTNodeResult::Failed;
    
		// 3. 동적 머티리얼 인스턴스(MID) 생성 또는 가져오기
		//    0은 첫 번째 머티리얼 슬롯을 의미합니다. 은신 효과를 적용할 머티리얼 슬롯 인덱스로 변경하세요.
		UMaterialInstanceDynamic* DynamicMaterial = MyMesh->CreateAndSetMaterialInstanceDynamic(0);

		if (DynamicMaterial)
		{
			// 4. MID 파라미터 값 변경하기
			//    "StealthAmount"는 머티리얼 에디터에서 생성한 스칼라 파라미터의 이름입니다.
			//    이 이름을 실제 파라미터 이름으로 변경해야 합니다.
			DynamicMaterial->SetScalarParameterValue(TEXT("StealthAmount"), 0.05f); // 1.0f를 은신 상태 값으로 가정
        
			// 성공적으로 실행되었음을 알림
			return EBTNodeResult::Succeeded;
		}

		// MID 생성 또는 파라미터 설정에 실패한 경우
		return EBTNodeResult::Failed;
	}

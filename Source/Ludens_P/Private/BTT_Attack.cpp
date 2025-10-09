// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Attack.h"

#include "AIController.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

UBTT_Attack::UBTT_Attack()
{
	NodeName = TEXT("Attack(Melee)");
	bNotifyTaskFinished = true;
	
}
EBTNodeResult::Type UBTT_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// AI 컨트롤러와 컨트롤하는 폰(캐릭터)을 가져옵니다.
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	ACharacter* ControlledPawn = Cast<ACharacter>(AIController->GetPawn());
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}
	
	// 유효한 공격 애니메이션 몽타주가 없으면 실패 처리합니다.
	if (!AttackAnim)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTT_Attack: AttackAnim is not set."));
		return EBTNodeResult::Failed;
	}

	// 애님 인스턴스를 가져옵니다.
	UAnimInstance* AnimInstance = ControlledPawn->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return EBTNodeResult::Failed;
	}
	
	// 이미 몽타주가 재생 중인지 확인하고, 그렇다면 실패 처리하여 중복 실행을 방지합니다.
	if (AnimInstance->Montage_IsPlaying(AttackAnim))
	{
		return EBTNodeResult::Failed;
	}

	// 몽타주 재생이 끝나면 호출될 델리게이트(OnAttackMontageEnded)를 바인딩합니다.
	FOnMontageEnded MontageEndedDelegate;
	// 델리게이트에 OwnerComp를 함께 넘겨주기 위해 BindUObject의 인자로 추가합니다.
	MontageEndedDelegate.BindUObject(this, &UBTT_Attack::OnAttackMontageEnded, &OwnerComp);
	
	// 몽타주를 재생합니다.
	AnimInstance->Montage_Play(AttackAnim);
	AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, AttackAnim);
	
	// 애니메이션이 끝날 때까지 기다려야 하므로 InProgress를 반환합니다.
	// OnAttackMontageEnded 함수에서 작업이 완료되면 FinishLatentTask를 호출하여 태스크를 종료시킵니다.
	return EBTNodeResult::InProgress;
}

void UBTT_Attack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	// 만약 몽타주가 중간에 중단되었다면(bInterrupted == true), 태스크를 실패 처리합니다.
	if (bInterrupted)
	{
		if (OwnerComp && OwnerComp->IsActive())
		{
			FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
		}
		return;
	}
	
	// AI 컨트롤러와 폰을 다시 가져옵니다.
	AAIController* AIController = OwnerComp->GetAIOwner();
	APawn* ControlledPawn = AIController ? AIController->GetPawn() : nullptr;

	if (!ControlledPawn)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 1. 공격 VFX 재생
	if (AttackVFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			AttackVFX,
			ControlledPawn->GetActorLocation(),
			ControlledPawn->GetActorRotation()
		);
	}

	// 2. 범위 데미지 적용
	TArray<AActor*> OverlappedActors;
	// 감지할 액터의 오브젝트 타입을 Pawn으로 한정합니다.
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn)); 
	
	// 자신을 중심으로 구체(Sphere)를 그려 겹치는 액터를 찾습니다.
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		ControlledPawn->GetActorLocation(),
		AttackRadius,
		ObjectTypes,
		nullptr, // 필터링할 액터 클래스 (예: APlayerCharacter::StaticClass())
		TArray<AActor*>{ControlledPawn}, // 자기 자신은 검출에서 제외
		OverlappedActors
	);
	
	// 겹친 모든 액터에게 데미지를 적용합니다.
	for (AActor* OverlappedActor : OverlappedActors)
	{
		UGameplayStatics::ApplyDamage(
			OverlappedActor,      // 데미지를 입을 액터
			DamageAmount,         // 데미지 양
			AIController,         // 데미지를 가한 컨트롤러
			ControlledPawn,       // 데미지를 유발한 액터 (가해자)
			UDamageType::StaticClass() // 기본 데미지 타입
		);
	}
	
	// 모든 작업이 성공적으로 끝났으므로 태스크를 성공으로 종료합니다.
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

void UBTT_Attack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	// 태스크가 어떤 이유로든 (성공, 실패, 중단) 종료될 때, 델리게이트 바인딩을 정리하고 몽타주를 멈춥니다.
	// 이는 다른 노드로 제어가 넘어갔을 때(Aborted) 공격 판정이 발생하는 것을 막아줍니다.
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController)
	{
		ACharacter* ControlledPawn = Cast<ACharacter>(AIController->GetPawn());
		if (ControlledPawn)
		{
			UAnimInstance* AnimInstance = ControlledPawn->GetMesh()->GetAnimInstance();
			if (AnimInstance && AttackAnim)
			{
				AnimInstance->Montage_Stop(0.1f, AttackAnim);
				//AnimInstance->Montage_ClearEndDelegate(FOnMontageEnded(), AttackAnim);
			}
		}
	}
}

void UBTT_Attack::Multicast_PlayerAttackEffects_Implementation(UAnimMontage* AttackAnim, UNiagaraSystem* AttackVFX)
{
}

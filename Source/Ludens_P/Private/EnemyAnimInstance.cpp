// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimInstance.h"

#include "EnemyMeleeComp.h"
#include "MagicComponent.h"
#include "Ludens_P/ShooterCombatComponent.h"

void UEnemyAnimInstance::AnimNotify_MeleeAttack()
{
	AActor* Owner = TryGetPawnOwner();
	if (Owner)
	{
		UEnemyMeleeComp* MeleeComp = Owner->GetComponentByClass<UEnemyMeleeComp>();
		if (MeleeComp)
		{
			MeleeComp->Attack();
		}
	}
}
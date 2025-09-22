// Fill out your copyright notice in the Description page of Project Settings.


#include "Reward_HealthBoost.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"

void UReward_HealthBoost::ApplyReward_Implementation(ACharacter * Player)
{
    if (!Player) return;

    // ü�� ���� ���� (��: Ŀ���� ĳ���� Ŭ������ �ʿ��� ���� ����)
    UE_LOG(LogTemp, Warning, TEXT("HP Increased!"));

    // ����: ü�� 50 ���� - ���� ĳ���� Ŭ������ ���� �ٸ��� �����ϼ���
    // ĳ���Ͱ� Ŀ�����̶�� ĳ���� �ʿ�: AMyCharacter* MyChar = Cast<AMyCharacter>(Player);
}

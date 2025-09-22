// Fill out your copyright notice in the Description page of Project Settings.


#include "Reward_HealthBoost.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"

void UReward_HealthBoost::ApplyReward_Implementation(ACharacter * Player)
{
    if (!Player) return;

    // 체력 증가 로직 (예: 커스텀 캐릭터 클래스가 필요할 수도 있음)
    UE_LOG(LogTemp, Warning, TEXT("HP Increased!"));

    // 예시: 체력 50 증가 - 실제 캐릭터 클래스에 따라 다르게 구현하세요
    // 캐릭터가 커스텀이라면 캐스팅 필요: AMyCharacter* MyChar = Cast<AMyCharacter>(Player);
}

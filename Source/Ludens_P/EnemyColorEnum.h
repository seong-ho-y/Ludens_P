// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyColorEnum.generated.h"

/**
 * 
 */
// 적의 색을 저장하는 enum
UENUM(Meta = (Bitflags, DisplayName = "Enemy Colors"))
enum class EEnemyColor : uint8
{
	White = 0 UMETA(DisplayName = "White"),
	Blue = 1 << 0 UMETA(DisplayName = "Blue"), // 0000 0001
	Green = 1 << 1 UMETA(DisplayName = "Green"), // 0000 0010
	Red = 1 << 2 UMETA(DisplayName = "Red"), // 0000 0100
	Cyan = Blue | Green UMETA(DisplayName = "Cyan"), // 0000 0011
	Magenta = Blue | Red UMETA(DisplayName = "Magenta"), // 0000 0101
	Yellow = Red | Green UMETA(DisplayName = "Yellow"), // 0000 0110
	Black = Blue | Green | Red UMETA(DisplayName = "Black"), // 0000 0111
};
ENUM_CLASS_FLAGS(EEnemyColor) // 비트 연산자 오버로딩을 위한 매크로

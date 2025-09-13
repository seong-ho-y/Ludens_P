// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "EEnemyColor.generated.h"

/**
 * 
 */
// 적의 색을 저장하는 enum

UENUM(BlueprintType)
enum class EEnemyColor : uint8
{
	// 기본색 (Primary)
	Red     UMETA(DisplayName = "Red"),
	Green   UMETA(DisplayName = "Green"),
	Blue    UMETA(DisplayName = "Blue"),

	// 혼합색 (Secondary)
	Yellow  UMETA(DisplayName = "Yellow"),  // Red + Green
	Cyan    UMETA(DisplayName = "Cyan"),    // Green + Blue
	Magenta UMETA(DisplayName = "Magenta"), // Red + Blue
	
	Black   UMETA(DisplayName = "Black")    // Red + Green + Blue
};
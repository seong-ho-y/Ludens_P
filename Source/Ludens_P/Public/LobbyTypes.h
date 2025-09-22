
// Source/Ludens_P/Public/LobbyTypes.h
#pragma once
#include "CoreMinimal.h"
#include "LobbyTypes.generated.h"

UENUM(BlueprintType)
enum class ELobbyColor : uint8
{
    None  UMETA(DisplayName = "None"),
    Red   UMETA(DisplayName = "Red"),
    Green UMETA(DisplayName = "Green"),
    Blue  UMETA(DisplayName = "Blue")
};

USTRUCT(BlueprintType)
struct FLobbyColorSlots
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int32 RedOwnerPlayerId = INDEX_NONE;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int32 GreenOwnerPlayerId = INDEX_NONE;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly) int32 BlueOwnerPlayerId = INDEX_NONE;
};

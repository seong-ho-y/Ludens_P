// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "Ludens_P/EEnemyColor.h"
#include "LobbyTypes.h"
#include "LobbyGameMode.generated.h"

class APlayerState_Real;

UCLASS()
class LUDENS_P_API ALobbyGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    ALobbyGameMode();

    // ���� S3~S5���� ���� ����: Ready/Unready/Start ������
    UFUNCTION(BlueprintCallable, Category = "Lobby")
    void StartGameIfAllReady();


    UPROPERTY(EditDefaultsOnly, Category = "Travel")
    TSoftObjectPtr<UWorld> StageMap;

};

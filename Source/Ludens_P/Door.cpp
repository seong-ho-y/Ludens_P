// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADoor::ADoor()
{
 	
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADoor::Open()
{
    if (!bIsOpen)
    {
        bIsOpen = true;

        // TODO: �� �ִϸ��̼� or �浹 ��Ȱ��ȭ
        SetActorHiddenInGame(false);
        SetActorEnableCollision(true);
    }
}

void ADoor::Close()
{
    if (bIsOpen)
    {
        bIsOpen = false;

        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);
    }
}
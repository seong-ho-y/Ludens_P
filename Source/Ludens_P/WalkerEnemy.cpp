#include "WalkerEnemy.h"
#include "WalkerAIComponent.h"

AWalkerEnemy::AWalkerEnemy()
{
	WalkerAI = CreateDefaultSubobject<UWalkerAIComponent>(TEXT("WalkerAI"));
}

void AWalkerEnemy::SetActive(bool bNewActive)
{
	Super::SetActive(bNewActive);

	if (WalkerAI)
	{
		WalkerAI->SetComponentTickEnabled(bNewActive);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("❌ WalkerAI is nullptr in SetActive!"));
	}
}

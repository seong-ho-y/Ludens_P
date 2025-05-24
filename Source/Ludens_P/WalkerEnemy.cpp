#include "WalkerEnemy.h"
#include "WalkerAIComponent.h"
// Walker형 적들의 코드
// 근접 공격을 하는 적들
// BP로 만들어서 사용

AWalkerEnemy::AWalkerEnemy()
{
	WalkerAI = CreateDefaultSubobject<UWalkerAIComponent>(TEXT("WalkerAI"));
}


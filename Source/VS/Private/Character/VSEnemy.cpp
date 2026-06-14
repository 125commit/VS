// LAvid


#include "Character/VSEnemy.h"

AVSEnemy::AVSEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AVSEnemy::SetIsElite(bool bIsInElite)
{
	bIsElite = bIsInElite;
	
	//TODO: 这里可以顺便设置精英怪，可以让它血更厚、体型更大等等
}

//再次从对象池中拿出来循环用
void AVSEnemy::OnRecycled()
{
	SetIsElite(false);
	//TODO:恢复生命值
}

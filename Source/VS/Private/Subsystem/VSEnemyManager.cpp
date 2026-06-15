// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/VSEnemyManager.h"

#include "Pool/VSObjectPool.h"
#include "Character/VSEnemy.h"

void UVSEnemyManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	EnemyPool = NewObject<UVSObjectPool>(this);
}

//退出关卡时清空
void UVSEnemyManager::Deinitialize()
{
	TArray<AActor*> EnemiesToReturn = MoveTemp(ActiveEnemies);
	
	for (AActor* Actor : ActiveEnemies)
	{
		if (EnemyPool && Actor)
		{
			EnemyPool->ReturnActorToPool(Actor);
		}
		
	}
	
	if (EnemyPool)
	{
		EnemyPool->ClearPool();
	}
	ActiveEnemies.Empty();
	
	Super::Deinitialize();
}

AActor* UVSEnemyManager::SpawnEnemiesFromPool(TSubclassOf<AActor> EnemyClass, const FVector& Location)
{
	if (!EnemyPool) return nullptr;
	
	AActor* NewEnemy =  EnemyPool->GetActorFromPool(GetWorld(), EnemyClass, Location, FRotator::ZeroRotator);
	if (NewEnemy && !ActiveEnemies.Contains(NewEnemy))
	{
		ActiveEnemies.Add(NewEnemy);
	}
	return NewEnemy;
}

void UVSEnemyManager::ReturnEnemiesToPool(AActor* Enemy)
{
	if (!EnemyPool || !Enemy) return;
	ActiveEnemies.RemoveSingleSwap(Enemy);
	EnemyPool->ReturnActorToPool(Enemy);
}

int32 UVSEnemyManager::GetActiveNormalEnemiesCount()
{
	return ActiveEnemies.Num();
}

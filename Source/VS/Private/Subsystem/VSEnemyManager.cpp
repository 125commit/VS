// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/VSEnemyManager.h"

#include "Pool/VSObjectPool.h"
#include "Character/VSEnemy.h"

void UVSEnemyManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	EnemyPool = NewObject<UVSObjectPool>(this);
}

void UVSEnemyManager::Deinitialize()
{
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
	if (NewEnemy)
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
	int32 Count = 0;
	for (AActor* EnemyActor : ActiveEnemies)
	{
		if (AVSEnemy* Enemy = Cast<AVSEnemy>(EnemyActor))
		{
			bool bIsNormal = !Enemy->GetIsElite();
			if (bIsNormal)
			{
				Count++;
			}
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("Enemy Num: %d"), Count));
	return Count;
}

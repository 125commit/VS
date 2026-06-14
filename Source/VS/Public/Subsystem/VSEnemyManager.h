// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "VSEnemyManager.generated.h"

class UVSObjectPool;
/**
 * 
 */
UCLASS()
class VS_API UVSEnemyManager : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	AActor* SpawnEnemiesFromPool(TSubclassOf<AActor> EnemyClass, const FVector& Location);
	void ReturnEnemiesToPool(AActor* Enemy);
	
	int32 GetActiveNormalEnemiesCount();
	
	UPROPERTY()
	TArray<AActor*> ActiveEnemies;
	
private:
	UPROPERTY()
	TObjectPtr<UVSObjectPool> EnemyPool;
	
	

};

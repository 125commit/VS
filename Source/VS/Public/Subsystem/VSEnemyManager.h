// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "VSEnemyManager.generated.h"

class AVSEnemy;
class UVSObjectPool;

UCLASS()
class VS_API UVSEnemyManager : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	//仅在运行时触发 Tick
	virtual bool IsTickableInEditor() const override { return false; }

	AActor* SpawnEnemiesFromPool(TSubclassOf<AActor> EnemyClass, const FVector& Location);
	void ReturnEnemiesToPool(AActor* Enemy);
	
	int32 GetActiveNormalEnemiesCount() const;
	int32 GetActiveEliteEnemiesCount() const;

	UPROPERTY()
	TArray<AActor*> ActiveEnemies;
	
	void OnEnemyDie(AVSEnemy* Enemy);

private:
	void ProcessEnemyLogic(float DeltaTime);
	void MoveToTarget(AVSEnemy* Enemy, const FVector& PlayerLoc, float DeltaTime);

	UPROPERTY()
	TObjectPtr<UVSObjectPool> EnemyPool;

	const float AttackRadius = 100.f;
	float AttackRadiusSq = 0.f;
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/VSEnemyManager.h"

#include "Pool/VSObjectPool.h"
#include "Character/VSEnemy.h"
#include "Data/Subsystem/DA_EnemyDropTable.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystem/VSDropManager.h"


void UVSEnemyManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	EnemyPool = NewObject<UVSObjectPool>(this);
	AttackRadiusSq = AttackRadius * AttackRadius;
}

void UVSEnemyManager::Deinitialize()
{
	TArray<AActor*> EnemiesToReturn = MoveTemp(ActiveEnemies);

	for (AActor* Actor : EnemiesToReturn)
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

//追踪 EnemyManager 使用 Tick 的性能消耗
TStatId UVSEnemyManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UVSEnemyManager, STATGROUP_Tickables);
}

void UVSEnemyManager::Tick(float DeltaTime)
{
	ProcessEnemyLogic(DeltaTime);
}

AActor* UVSEnemyManager::SpawnEnemiesFromPool(TSubclassOf<AActor> EnemyClass, const FVector& Location)
{
	if (!EnemyPool) return nullptr;

	AActor* NewEnemy = EnemyPool->GetActorFromPool(GetWorld(), EnemyClass, Location, FRotator::ZeroRotator);
	if (!NewEnemy || ActiveEnemies.Contains(NewEnemy)) return NewEnemy;

	if (AVSEnemy* VSEnemy = Cast<AVSEnemy>(NewEnemy))
	{
		VSEnemy->ResetForSpawn();
		ActiveEnemies.Add(NewEnemy);
	}

	return NewEnemy;
}

void UVSEnemyManager::ReturnEnemiesToPool(AActor* Enemy)
{
	if (!EnemyPool || !Enemy) return;

	if (AVSEnemy* VSEnemy = Cast<AVSEnemy>(Enemy))
	{
		VSEnemy->OnRecycled();
	}

	ActiveEnemies.RemoveSingleSwap(Enemy);
	EnemyPool->ReturnActorToPool(Enemy);
}

int32 UVSEnemyManager::GetActiveNormalEnemiesCount() const
{
	int32 Count = 0;
	for (const AActor* Actor : ActiveEnemies)
	{
		if (const AVSEnemy* Enemy = Cast<AVSEnemy>(Actor))
		{
			if (!Enemy->GetIsElite())
			{
				++Count;
			}
		}
	}
	return Count;
}
int32 UVSEnemyManager::GetActiveEliteEnemiesCount() const
{
	int32 Count = 0;
	for (const AActor* Actor : ActiveEnemies)
	{
		if (const AVSEnemy* Enemy = Cast<AVSEnemy>(Actor))
		{
			if (Enemy->GetIsElite())
			{
				++Count;
			}
		}
	}
	return Count;
}

void UVSEnemyManager::OnEnemyDie(AVSEnemy* Enemy)
{
	if (!Enemy || !ActiveEnemies.Contains(Enemy)) return;

	UVSDropManager* DropManager = GetWorld()->GetSubsystem<UVSDropManager>();
	if (!DropManager) return;

	if (UDA_EnemyDropTable* DropTable = Enemy->GetDropTable())
	{
		DropManager->SpawnDrop(Enemy->GetActorLocation(), DropTable);
	}
	ReturnEnemiesToPool(Enemy);
}

void UVSEnemyManager::ProcessEnemyLogic(float DeltaTime)
{
	if (ActiveEnemies.Num() == 0) return;

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!Player) return;

	const FVector PlayerLoc = Player->GetActorLocation();

	//避免遍历时把敌人放回池中(死亡)，所以先创建一个本地副本
	const TArray<AActor*> EnemiesSnapshot = ActiveEnemies;
	for (AActor* EnemyActor : EnemiesSnapshot)
	{
		AVSEnemy* Enemy = Cast<AVSEnemy>(EnemyActor);
		if (!Enemy || Enemy->IsDead()) continue;

		const float DistanceSq = FVector::DistSquared(PlayerLoc, Enemy->GetActorLocation());

		if (DistanceSq <= AttackRadiusSq)
		{
			Enemy->SetVisualSpeed(0.f);
			//TODO:对玩家造成伤害
		}
		else
		{
			MoveToTarget(Enemy, PlayerLoc, DeltaTime);
		}
	}
}

void UVSEnemyManager::MoveToTarget(AVSEnemy* Enemy, const FVector& PlayerLoc, float DeltaTime)
{
	const FVector LastLoc = Enemy->GetActorLocation();

	FVector Direction = PlayerLoc - LastLoc;
	Direction.Z = 0.f;
	if (Direction.IsNearlyZero()) return;

	Direction.Normalize();
	const float Speed = Enemy->MoveSpeed;

	const FVector NewLoc = LastLoc + Direction * Speed * DeltaTime;
	Enemy->SetActorLocation(NewLoc, true);
	Enemy->SetActorRotation(FRotator(0.f, Direction.Rotation().Yaw, 0.f));
	
	Enemy->SetVisualSpeed(Speed);
}

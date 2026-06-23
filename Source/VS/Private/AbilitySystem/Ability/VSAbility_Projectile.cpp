// LAvid


#include "AbilitySystem/Ability/VSAbility_Projectile.h"

#include "VSGameplayTags.h"
#include "Actor/VS_WeaponActor.h"
#include "Subsystem/VSEnemyManager.h"
#include "Subsystem/VSWeaponSubsysem.h"
#include "Data/VSAbilityInfoData.h"
#include "Character/VSEnemy.h"

UVSAbility_Projectile::UVSAbility_Projectile()
{
	// C++ 默认当强能魔杖；火焰魔杖用蓝图子类改 Tag + TargetMode + MovementMode
	SetupWeaponAbility(FVSGameplayTags::Get().Abilities_Weapon_MagicWand);
}


bool UVSAbility_Projectile::TryGetWeaponFireContext(FVSWeaponFireContext& OutContext) const
{
	return Super::TryGetWeaponFireContext(OutContext);
}

void UVSAbility_Projectile::ExecuteFire(const FVSAbilityRuntimeStats& Stats)
{
	FVSWeaponFireContext Context;
	if (!TryGetWeaponFireContext(Context)) return;
	
	UVSEnemyManager* EnemyManager = Context.World -> GetSubsystem<UVSEnemyManager>();
	if (!EnemyManager) return;
	
	const FVector PlayerLoc = Context.AvatarActor->GetActorLocation();
	const float FireRangeSq = FMath::Square(FMath::Max(0.f, FireRange));
	
	//获取射程内的敌人
	TArray<AVSEnemy*> EnemiesInRange;
	EnemiesInRange.Reserve(EnemyManager->ActiveEnemies.Num()); 	//预分配内存
	for (AActor* Actor : EnemyManager->ActiveEnemies)
	{
		AVSEnemy* Enemy = Cast<AVSEnemy>(Actor);
		if (!Enemy || Enemy->IsDead()) continue;
		if (FVector::DistSquared(PlayerLoc, Enemy->GetActorLocation()) <= FireRangeSq)
		{
			EnemiesInRange.Add(Enemy);
		}
	}
	
	// 范围内没有敌人，则不生成任何 Projectile
	if (EnemiesInRange.Num() == 0)
	{
		return;
	}
	
	//MagicWand模式：根据距离给敌人排序
		//Sort是数组的一个排序方法；需要传入一个“比较规则”（即Predicate，类似是一个Lambda）
		// [&PlayerLoc] 是捕获列表
		//返回的是bool，表示 A 是否排在 B 前面
	if (TargetMode == EVSProjectileTargetMode::NearestEnemy)
	{
		EnemiesInRange.Sort([&PlayerLoc](const AVSEnemy& A, const AVSEnemy& B)
		{
			return FVector::DistSquared(PlayerLoc, A.GetActorLocation())
				 < FVector::DistSquared(PlayerLoc, B.GetActorLocation());
		});
	}
	
	
	//Spawn
	const int32 ProjectileCount = FMath::Max(1, Stats.ProjectileCount);
	
	for (int32 i = 0; i < ProjectileCount; i++)
	{
		AVSEnemy* TargetEnemy = nullptr;
		switch (TargetMode)
		{
			//如果是MagicWand，就找最近的
		case EVSProjectileTargetMode::NearestEnemy:
			TargetEnemy = EnemiesInRange.IsValidIndex(i) ? EnemiesInRange[i] : EnemiesInRange[0];
			break;
			
			//如果是FireWand，就随机
		case EVSProjectileTargetMode::RandomEnemy:
			TargetEnemy = EnemiesInRange[FMath::RandRange(0, EnemiesInRange.Num() - 1)];
			break;
		}
		
		
		FVSWeaponInitParams Params = MakeBaseWeaponParams(Stats);
		
		//设置 Params 中的 TargetEnemy（这是给Actor用的）
		if (MovementMode == EVSProjectileMovementMode::Homing)
		{
			Params.TargetEnemy = TargetEnemy;
		}
		else
		{
			Params.TargetEnemy = nullptr;
		}
		
		
		FVector InitialDirection = FVector();
		if (TargetEnemy)
		{
			InitialDirection = (TargetEnemy->GetActorLocation() - PlayerLoc).GetSafeNormal();
		}
		InitialDirection.Z = 0.f;
		InitialDirection = InitialDirection.GetSafeNormal();
		
		Params.MovementMode = MovementMode;
		Params.FlightDirection = InitialDirection;
		
		
		const FRotator SpawnRotation = InitialDirection.IsNearlyZero() ? Context.AvatarActor->GetActorRotation() : InitialDirection.Rotation();
		const FVector SpawnLocation = PlayerLoc + InitialDirection * SpawnForwardOffset;  //增加一个偏移量，使其在角色前面生成
		const FTransform SpawnTransform(SpawnRotation, SpawnLocation);
		
		
		Context.WeaponSubsystem->SpawnWeaponFromPool(
			WeaponActorClass,
			SpawnTransform,
			Context.AvatarActor,
			Params);
	}
	
}


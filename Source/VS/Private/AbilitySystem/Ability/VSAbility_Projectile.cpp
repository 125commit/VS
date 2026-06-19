// LAvid


#include "AbilitySystem/Ability/VSAbility_Projectile.h"

#include "VSGameplayTags.h"
#include "Actor/VS_WeaponActor.h"
#include "Subsystem/VSEnemyManager.h"
#include "Subsystem/VSWeaponSubsysem.h"
#include "Data/DA_AbilityInfo.h"
#include "Character/VSEnemy.h"

UVSAbility_Projectile::UVSAbility_Projectile()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	const FVSGameplayTags GameplayTags = FVSGameplayTags::Get();
	AbilityTag = GameplayTags.Abilities_Weapon_MagicWand;
	AbilityTypeTag = GameplayTags.Abilities_Type_Weapon;
}

void UVSAbility_Projectile::ExecuteFire(const FVSAbilityRuntimeStats& Stats)
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	UVSWeaponSubsysem* WeaponSubsystem = World->GetSubsystem<UVSWeaponSubsysem>();
	UVSEnemyManager* EnemyManager = World -> GetSubsystem<UVSEnemyManager>();
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	
	if (!WeaponActorClass || !AvatarActor || !WeaponSubsystem || !EnemyManager )
	{
		return;
	}
	
	
	const FVector PlayerLoc = AvatarActor->GetActorLocation();
	const float FireRangeSq = FMath::Square(FMath::Max(0.f, FireRange));
	
	
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
	
	// 范围内没有敌人，本轮不生成任何 Projectile
	if (EnemiesInRange.Num() == 0)
	{
		return;
	}
	
	//根据距离给敌人排序
	//Sort是数组的一个排序方法；需要传入一个“比较规则”（即Predicate，类似是一个Lambda）
	// [&PlayerLoc] 是捕获列表
	//返回的是bool，表示 A 是否排在 B 前面
	EnemiesInRange.Sort([&PlayerLoc](const AVSEnemy& A, const AVSEnemy& B)
	{
		const float DistSqA = FVector::DistSquared(PlayerLoc, A.GetActorLocation());
		const float DistSqB = FVector::DistSquared(PlayerLoc, B.GetActorLocation());
		return DistSqA < DistSqB;
	});
	
	
	const int32 ProjectileCount = FMath::Max(1, Stats.ProjectileCount);
	//Spawn MagicWand
	for (int32 i = 0; i < ProjectileCount; i++)
	{
		//从排序后的数组中找到TargetEnemy
		AVSEnemy* TargetEnemy = nullptr;
		if (EnemiesInRange.IsValidIndex(i))
		{
			TargetEnemy = EnemiesInRange[i];
		}
		else if (EnemiesInRange.Num() > 0)
		{
			TargetEnemy = EnemiesInRange[0];
		}
		
		//初始化数据
		FVSWeaponInitParams Params;
		Params.Damage = ComputeFinalDamage(Stats.BaseDamage);
		Params.ProjectileSpeed = Stats.ProjectileSpeed > 0.f ? Stats.ProjectileSpeed : 600.f;
		Params.TargetEnemy = TargetEnemy;
		Params.Duration = Stats.Duration > 0.f ? Stats.Duration : 3.f;

		
		FVector InitialDirection = AvatarActor->GetActorForwardVector();
		if (TargetEnemy)
		{
			InitialDirection = (TargetEnemy->GetActorLocation() - PlayerLoc).GetSafeNormal();
		}
		InitialDirection.Z = 0.f;
		InitialDirection = InitialDirection.GetSafeNormal();
		
		const FRotator SpawnRotation = InitialDirection.IsNearlyZero() ? AvatarActor->GetActorRotation() : InitialDirection.Rotation();
		const FVector SpawnLocation = PlayerLoc + InitialDirection * SpawnForwardOffset;  //增加一个偏移量，使其在角色前面生成
		const FTransform SpawnTransform(SpawnRotation, SpawnLocation);
		
		
		WeaponSubsystem->SpawnWeaponFromPool(
			WeaponActorClass,
			SpawnTransform,
			AvatarActor,
			Params);
	}
	
}

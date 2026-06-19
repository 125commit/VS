// LAvid


#include "AbilitySystem/Ability/VSAbility_Whip.h"
#include "VSGameplayTags.h"
#include "Actor/VSWhipActor.h"
#include "Data/DA_AbilityInfo.h"

UVSAbility_Whip::UVSAbility_Whip()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	const FVSGameplayTags GameplayTags = FVSGameplayTags::Get();
	AbilityTag = GameplayTags.Abilities_Weapon_Whip;
	AbilityTypeTag = GameplayTags.Abilities_Type_Weapon;
}

void UVSAbility_Whip::ExecuteFire(const FVSAbilityRuntimeStats& Stats)
{
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!WeaponActorClass || !AvatarActor) return;

	//鞭子的数量
	const int32 WhipCount = FMath::Max(1, Stats.ProjectileCount);
	
	//获取角色朝向的单位向量
	FVector Forward = AvatarActor->GetActorForwardVector();
	Forward.Z = 0.f;
	Forward = Forward.GetSafeNormal();
	
	for (int32 i = 0; i < WhipCount; i++)
	{
		//设置Whhip朝向
		FVector FacignDirection = (i % 2 == 0) ? Forward : -Forward;
		const FRotator FacingRotation = FacignDirection.Rotation();
		const FTransform SpawningTransform(FacingRotation, AvatarActor->GetActorLocation());
	
		//TODO：从对象池中获取
		AVS_WeaponActor* Whip = GetWorld()->SpawnActorDeferred<AVSWhipActor>(
			WeaponActorClass,
			SpawningTransform,
			AvatarActor,
			Cast<APawn>(AvatarActor),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);
		
		if (!Whip) continue;
	
		FVSWeaponInitParams Params;
		Params.Area = Stats.Area;
		Params.Duration = Stats.Duration;
		Params.Damage = ComputeFinalDamage(Stats.BaseDamage);
		Params.FacingRotation = FacingRotation;
	
		Whip->InitFromParams(Params);
	
		//done
		Whip->FinishSpawning(SpawningTransform);
	}
	
	
	
}



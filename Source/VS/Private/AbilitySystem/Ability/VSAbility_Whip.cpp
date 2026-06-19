// LAvid


#include "AbilitySystem/Ability/VSAbility_Whip.h"
#include "VSGameplayTags.h"
#include "Actor/VSWhipActor.h"
#include "Data/DA_AbilityInfo.h"
#include "Subsystem/VSWeaponSubsysem.h"

UVSAbility_Whip::UVSAbility_Whip()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	const FVSGameplayTags GameplayTags = FVSGameplayTags::Get();
	AbilityTag = GameplayTags.Abilities_Weapon_Whip;
	AbilityTypeTag = GameplayTags.Abilities_Type_Weapon;
}

void UVSAbility_Whip::ExecuteFire(const FVSAbilityRuntimeStats& Stats)
{
	UWorld* World = GetWorld();
	UVSWeaponSubsysem* WeaponSubsystem = World->GetSubsystem<UVSWeaponSubsysem>();
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!WeaponActorClass || !AvatarActor || !WeaponSubsystem || !World) return;
	
	//鞭子的数量
	const int32 WhipCount = FMath::Max(1, Stats.ProjectileCount);
	
	//获取角色朝向的单位向量
	FVector Forward = AvatarActor->GetActorForwardVector();
	Forward.Z = 0.f;
	Forward = Forward.GetSafeNormal();
	
	for (int32 i = 0; i < WhipCount; i++)
	{
		//设置Whip朝向
		FVector FacingDirection = (i % 2 == 0) ? Forward : -Forward;
		const FRotator FacingRotation = FacingDirection.Rotation();
		const FTransform SpawningTransform(FacingRotation, AvatarActor->GetActorLocation());
	
		FVSWeaponInitParams Params;
		Params.Area = Stats.Area;
		Params.Duration = Stats.Duration;
		Params.Damage = ComputeFinalDamage(Stats.BaseDamage);
		Params.FacingRotation = FacingRotation;
		
		WeaponSubsystem->SpawnWeaponFromPool(
			WeaponActorClass,
			SpawningTransform,
			AvatarActor,
			Params);
	}
	
	
	
}



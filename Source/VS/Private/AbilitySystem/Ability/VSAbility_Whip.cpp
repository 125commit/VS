// LAvid


#include "AbilitySystem/Ability/VSAbility_Whip.h"
#include "VSGameplayTags.h"
#include "Actor/VSWhipActor.h"
#include "Data/DA_AbilityInfo.h"
#include "Subsystem/VSWeaponSubsysem.h"

UVSAbility_Whip::UVSAbility_Whip()
{
	SetupWeaponAbility(FVSGameplayTags::Get().Abilities_Weapon_Whip);
}

void UVSAbility_Whip::ExecuteFire(const FVSAbilityRuntimeStats& Stats)
{
	FVSWeaponFireContext Context;
	if (!TryGetWeaponFireContext(Context)) return;
	
	//鞭子的数量
	const int32 WhipCount = FMath::Max(1, Stats.ProjectileCount);
	
	//获取角色朝向的单位向量
	FVector Forward = Context.AvatarActor->GetActorForwardVector();
	Forward.Z = 0.f;
	Forward = Forward.GetSafeNormal();
	
	for (int32 i = 0; i < WhipCount; i++)
	{
		//设置Whip朝向
		FVector FacingDirection = (i % 2 == 0) ? Forward : -Forward;
		const FRotator FacingRotation = FacingDirection.Rotation();
		const FTransform SpawningTransform(FacingRotation, Context.AvatarActor->GetActorLocation());
		
		FVSWeaponInitParams Params = MakeBaseWeaponParams(Stats);
		Params.FacingRotation = FacingRotation;
		
		Context.WeaponSubsystem->SpawnWeaponFromPool(
			WeaponActorClass,
			SpawningTransform,
			Context.AvatarActor,
			Params);
	}
	
	
	
}

bool UVSAbility_Whip::TryGetWeaponFireContext(FVSWeaponFireContext& OutContext) const
{
	return Super::TryGetWeaponFireContext(OutContext);
}



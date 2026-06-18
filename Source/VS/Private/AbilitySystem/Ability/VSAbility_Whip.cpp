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
	if (!WhipActor || !GetAvatarActorFromActorInfo()) return;
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	const FTransform& SpawningTransform = AvatarActor->GetTransform();
	
	AVS_WeaponActor* Whip = GetWorld()->SpawnActorDeferred<AVSWhipActor>(
		WeaponActorClass,
		SpawningTransform,
		AvatarActor,
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (!Whip) return;
	
	FVSWeaponInitParams Params;
	Params.Area = Stats.Area;
	Params.Duration = Stats.Duration;
	Params.Damage = ComputeFinalDamage(Stats.BaseDamage);
	Params.FacingRotation = AvatarActor->GetActorRotation();
	
	Whip->InitFromParams(Params);
	
	//done
	Whip->FinishSpawning(SpawningTransform);
	
}



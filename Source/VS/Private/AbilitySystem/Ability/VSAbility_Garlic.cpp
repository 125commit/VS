// LAvid


#include "AbilitySystem/Ability/VSAbility_Garlic.h"
#include "Actor/VS_WeaponActor.h"
#include "VSGameplayTags.h"
#include "Subsystem/VSWeaponSubsysem.h"
#include "Actor/VSGarlicActor.h"

UVSAbility_Garlic::UVSAbility_Garlic()
{
	SetupWeaponAbility(FVSGameplayTags::Get().Abilities_Weapon_Garlic);
}

void UVSAbility_Garlic::ExecuteFire(const FVSAbilityRuntimeStats& Stats)
{
	FVSWeaponFireContext Context;
	if (!TryGetWeaponFireContext(Context)) return;
	
	const FVSWeaponInitParams Params = MakeBaseWeaponParams(Stats);
	
	AVSGarlicActor* GarlicActor = Cast<AVSGarlicActor>(
			Context.WeaponSubsystem->FindActiveWeapon(Context.AvatarActor, WeaponActorClass));

	if (!GarlicActor)
	{
		const FTransform SpawnTransform(Context.AvatarActor->GetActorRotation(), Context.AvatarActor->GetActorLocation());
		
		GarlicActor = Cast<AVSGarlicActor>(
			Context.WeaponSubsystem->SpawnWeaponFromPool( WeaponActorClass, SpawnTransform, Context.AvatarActor, Params));
	}
	
	if (!GarlicActor) return;
	
	GarlicActor->ApplyPeriodicDamageInCircle(Params);
}

void UVSAbility_Garlic::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	//整个技能结束，回池
	if (HasAuthority(&ActivationInfo))
	{
		if (UWorld* World = GetWorld())
		{
			if (UVSWeaponSubsysem* WeaponSubsystem = World->GetSubsystem<UVSWeaponSubsysem>())
			{
				if (AActor* Avatar = GetAvatarActorFromActorInfo())
				{
					if (AVS_WeaponActor* GarlicActor = WeaponSubsystem->FindActiveWeapon(Avatar, WeaponActorClass))
					{
						WeaponSubsystem->ReturnWeaponToPool(GarlicActor);
					}	
				}
			}
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

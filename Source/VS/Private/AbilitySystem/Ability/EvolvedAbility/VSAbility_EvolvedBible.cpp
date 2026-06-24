// LAvid


#include "AbilitySystem/Ability/EvolvedAbility/VSAbility_EvolvedBible.h"
#include "Actor/VS_WeaponActor.h"
#include "VSGameplayTags.h"
#include "Data/VSAbilityInfoData.h"
#include "Subsystem/VSWeaponSubsysem.h"

UVSAbility_EvolvedBible::UVSAbility_EvolvedBible()
{
	SetupWeaponAbility(FVSGameplayTags::Get().Abilities_Weapon_EvolvedBook);
}

void UVSAbility_EvolvedBible::OnWeaponFire()
{
	// 只在激活时生成一次书，不创建 WaitDelay → 不进入循环
	if (!GetAvatarActorFromActorInfo() || !GlobalAbilityInfoData) return;
	const FVSAbilityRuntimeStats Stats = GlobalAbilityInfoData->ComputeAbilityStats(
		AbilityTag, GetAbilityLevel(), GetAbilitySystemComponentFromActorInfo());
	
	ExecuteFire(Stats);
}

void UVSAbility_EvolvedBible::ExecuteFire(const FVSAbilityRuntimeStats& Stats)
{
	// 已经有存活的常驻书就不再重复生成
	for (const TWeakObjectPtr<AVS_WeaponActor>& Book : SpawnedBooks)
	{
		if (Book.IsValid()) return;
	}
	
	FVSWeaponFireContext Context;
	if (!TryGetWeaponFireContext(Context)) return;
	
	const int32 Count = FMath::Max(1, Stats.ProjectileCount);
	const FVector PlayerLoc = Context.AvatarActor->GetActorLocation();
	
	SpawnedBooks.Reset();
	
	for (int32 i = 0; i < Count; ++i)
	{
		// 数据表里 Duration 配 0 → BibleActor 不设生命周期 → 永久常驻
		FVSWeaponInitParams Params = MakeBaseWeaponParams(Stats);
		
		const float AngleDeg = 360.f / Count * i;
		Params.FacingRotation = FRotator(0.f, AngleDeg, 0.f);
		const FTransform SpawnTransform(Params.FacingRotation, PlayerLoc);
		
		AVS_WeaponActor* Book = Context.WeaponSubsystem->SpawnWeaponFromPool(
			WeaponActorClass, 
			SpawnTransform, 
			Context.AvatarActor, 
			Params);
	
		if (Book)
		{
			SpawnedBooks.Add(Book);
		}
	}
}

void UVSAbility_EvolvedBible::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo ,bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		if (UVSWeaponSubsysem* WeaponSubsystem = World->GetSubsystem<UVSWeaponSubsysem>())
		{
			for (const TWeakObjectPtr<AVS_WeaponActor>& Book : SpawnedBooks)
			{
				if (Book.IsValid())
				{
					WeaponSubsystem->ReturnWeaponToPool(Book.Get());
				}
			}
		}
	}
	SpawnedBooks.Reset();

}



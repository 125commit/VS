// LAvid


#include "AbilitySystem/Ability/EvolvedAbility/VSAbility_EvolvedWhip.h"

#include "VSGameplayTags.h"
#include "VS_AbilitySystemLibrary.h"
#include "Subsystem/VSEnemyManager.h"
#include "Actor/VS_WeaponActor.h"

UVSAbility_EvolvedWhip::UVSAbility_EvolvedWhip()
{
	SetupWeaponAbility(FVSGameplayTags::Get().Abilities_Weapon_EvolvedWhip);
}

void UVSAbility_EvolvedWhip::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!HasAuthority(&CurrentActivationInfo)) return;
	
	if (UWorld* World = GetWorld())
	{
		if (UVSEnemyManager* EnemyManager = World->GetSubsystem<UVSEnemyManager>())
		{
			// 给 OnEnemyDamagedDelegate 绑定函数，攻击时回血
			DamagedHandle = EnemyManager->OnEnemyDamagedDelegate.AddUObject(this, &UVSAbility_EvolvedWhip::HandleEnemyDamaged);
		}
	}
}

void UVSAbility_EvolvedWhip::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		if (UVSEnemyManager* EnemyManager = World->GetSubsystem<UVSEnemyManager>())
		{
			//技能结束，移除委托
			EnemyManager->OnEnemyDamagedDelegate.Remove(DamagedHandle);
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

/*
 * 只需重写 ComputeFinalDamage，而不用重写调用它的 MakeBaseWeaponParams 函数（设置技能数据）
 * 这是因为 —— 虚函数动态分发是看对象的运行时类型，而不是看"调用语句写在哪个类里
 */
float UVSAbility_EvolvedWhip::ComputeFinalDamage(float BaseDamage) const
{
	return Super::ComputeFinalDamage(BaseDamage) * CritMultiplier;
}

void UVSAbility_EvolvedWhip::HandleEnemyDamaged(AVSEnemy* Victim, float Damage, AActor* Causer)
{
	// !Causer->IsA(WeaponActorClass) 确保是自己的 Actor
	if (!Causer || !WeaponActorClass || !Causer->IsA(WeaponActorClass)) return;
	if (Causer->GetOwner() != GetAvatarActorFromActorInfo()) return;
	
	UVS_AbilitySystemLibrary::HealActor(GetAvatarActorFromActorInfo(), HealPerHit);
}

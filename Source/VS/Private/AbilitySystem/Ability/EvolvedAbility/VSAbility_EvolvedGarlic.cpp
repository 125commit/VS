// LAvid


#include "AbilitySystem/Ability/EvolvedAbility/VSAbility_EvolvedGarlic.h"

#include "VSGameplayTags.h"
#include "VS_AbilitySystemLibrary.h"
#include "Subsystem/VSEnemyManager.h"
#include "Actor/VS_WeaponActor.h"

UVSAbility_EvolvedGarlic::UVSAbility_EvolvedGarlic()
{
	SetupWeaponAbility(FVSGameplayTags::Get().Abilities_Weapon_EvolvedGarlic);
}

void UVSAbility_EvolvedGarlic::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!HasAuthority(&CurrentActivationInfo)) return;
	if (UWorld* World = GetWorld())
	{
		if (UVSEnemyManager* EM = World->GetSubsystem<UVSEnemyManager>())
		{
			KilledHandle = EM->OnEnemyKilledDelegate.AddUObject(this, &UVSAbility_EvolvedGarlic::HandleEnemyKilled);
		}
	}
}

void UVSAbility_EvolvedGarlic::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		if (UVSEnemyManager* EM = World->GetSubsystem<UVSEnemyManager>())
		{
			EM->OnEnemyKilledDelegate.Remove(KilledHandle);
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

float UVSAbility_EvolvedGarlic::ComputeFinalDamage(float BaseDamage) const
{
	return Super::ComputeFinalDamage(BaseDamage) + CurrentDamageBonus;
}

void UVSAbility_EvolvedGarlic::HandleEnemyKilled(AVSEnemy* Victim, AActor* Causer)
{
	// 只统计"自己的噬魂光环 "击杀
	if (!Causer || !WeaponActorClass || !Causer->IsA(WeaponActorClass)) return;
	if (Causer->GetOwner() != GetAvatarActorFromActorInfo()) return;
	
	// 击杀回血，累计"实际回复量"
	const float Healed = UVS_AbilitySystemLibrary::HealActor(GetAvatarActorFromActorInfo(), HealPerKill);
	AccumulatedHeal += Healed;
	
	// 每 HealPerDamageStep 点回血 → +1 伤害; 并且封顶 MaxDamageBonus
	const float Steps = FMath::FloorToFloat(AccumulatedHeal / HealPerDamageStep);
	CurrentDamageBonus = FMath::Min(Steps, MaxDamageBonus);
}

	
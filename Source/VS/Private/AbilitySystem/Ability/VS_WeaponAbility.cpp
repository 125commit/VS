// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/VS_WeaponAbility.h"
#include "Actor/VS_WeaponActor.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/VS_AttributeSet.h"
#include "Data/DA_AbilityInfo.h"



void UVS_WeaponAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 只要技能被激活，立刻开火一次，并进入大循环
	if (HasAuthority(&CurrentActivationInfo))
	{
		OnWeaponFire();
	}
}

void UVS_WeaponAbility::OnWeaponFire()
{
	if (!GetAvatarActorFromActorInfo() || !GlobalAbilityInfoData) return;
	
	FVSAbilityRuntimeStats Stats =  GlobalAbilityInfoData->ComputeAbilityStats(AbilityTag, GetAbilityLevel(),
		GetAbilitySystemComponentFromActorInfo(),false,false);
	
	// Start fire
	ExecuteFire(Stats);
	
	//Schedule Next Fire
	const float Delay = Stats.bNoCooldown ? 0.f : Stats.Cooldown;
	
	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, Delay);
	DelayTask->OnFinish.AddDynamic(this, &UVS_WeaponAbility::OnWeaponFire);
	DelayTask->ReadyForActivation();
}

void UVS_WeaponAbility::ExecuteFire(const FVSAbilityRuntimeStats& Stats)
{
	//给空的技能子类用
	//TODO：子类都做完之后可以删除它
	if (!WeaponActorClass || !GetAvatarActorFromActorInfo()) return;
	
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	const FTransform& SpawningTransform = AvatarActor->GetTransform();
	
	AVS_WeaponActor* Weapon = GetWorld()->SpawnActorDeferred<AVS_WeaponActor>(
		WeaponActorClass,
		SpawningTransform,
		AvatarActor,
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);
	
	if (!Weapon) return;
	
	FVSWeaponInitParams Params;
	Params.Area = Stats.Area;
	Params.Duration = Stats.Duration;
	Params.Damage = ComputeFinalDamage(Stats.BaseDamage);
	Params.FacingRotation = AvatarActor->GetActorRotation();
	
	Weapon->InitFromParams(Params);
	
	//done
	Weapon->FinishSpawning(SpawningTransform);
}

float UVS_WeaponAbility::ComputeFinalDamage(float BaseDamage) const
{
	const float Might = GetAttributeValue(UVS_AttributeSet::GetBaseDamageAttribute());
	return BaseDamage * Might;
}


float UVS_WeaponAbility::GetAttributeValue(const FGameplayAttribute& Attribute) const
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		if (ASC->HasAttributeSetForAttribute(Attribute))
		{
			return ASC->GetNumericAttribute(Attribute);
		}
	}
	return 1.0f; // 默认返回 1.0，保证逻辑不崩溃
}

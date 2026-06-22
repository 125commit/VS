// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/VS_WeaponAbility.h"
#include "Actor/VS_WeaponActor.h"
#include "AbilitySystemComponent.h"
#include "VSGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "AbilitySystem/VS_AttributeSet.h"
#include "Data/DA_AbilityInfo.h"
#include "Subsystem/VSWeaponSubsysem.h"



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
	
	// CHANGED: 原先固定 Delay = Cooldown → 改为走虚函数 GetNextFireDelay
	// 原因：让子类（如圣经）能定制开火节奏，而不破坏父类循环与其它武器
	const float Delay = GetNextFireDelay(Stats);
	
	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, Delay);
	DelayTask->OnFinish.AddDynamic(this, &UVS_WeaponAbility::OnWeaponFire);
	DelayTask->ReadyForActivation();
}

float UVS_WeaponAbility::GetNextFireDelay(const FVSAbilityRuntimeStats& Stats) const
{
	// 默认节奏：发射/周期间隔 = Cooldown（进化无冷却时为 0），保持其它武器原有行为不变
	return Stats.bNoCooldown ? 0.f : Stats.Cooldown;
}

void UVS_WeaponAbility::ExecuteFire(const FVSAbilityRuntimeStats& Stats)
{
	
}

float UVS_WeaponAbility::ComputeFinalDamage(float BaseDamage) const
{
	const float Might = GetAttributeValue(UVS_AttributeSet::GetMightAttribute());
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

void UVS_WeaponAbility::SetupWeaponAbility(const FGameplayTag& InAbilityTag)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	AbilityTag = InAbilityTag;
	
	const FVSGameplayTags GameplayTags = FVSGameplayTags::Get();
	AbilityTypeTag = GameplayTags.Abilities_Type_Weapon;
	
}

bool UVS_WeaponAbility::TryGetWeaponFireContext(FVSWeaponFireContext& OutContext) const
{
	OutContext.World = GetWorld();
	OutContext.AvatarActor = GetAvatarActorFromActorInfo();
	OutContext.WeaponSubsystem = OutContext.World ? GetWorld()->GetSubsystem<UVSWeaponSubsysem>() : nullptr;
	
	OutContext.bIsValid = WeaponActorClass
		 && OutContext.AvatarActor
		 && OutContext.WeaponSubsystem
		 && OutContext.World;
	
	return OutContext.bIsValid;
}

FVSWeaponInitParams UVS_WeaponAbility::MakeBaseWeaponParams(const FVSAbilityRuntimeStats& Stats) const
{
	FVSWeaponInitParams Params;
	Params.Damage = ComputeFinalDamage(Stats.BaseDamage);
	Params.Area = Stats.Area;
	Params.Duration = Stats.Duration;
	Params.ProjectileSpeed = Stats.ProjectileSpeed > 0.f ? Stats.ProjectileSpeed : 600.f;
	// 速度倍率兜底为 1.0，避免表未配时把速度归零
	Params.SpeedMultiplier = Stats.SpeedMultiplier > 0.f ? Stats.SpeedMultiplier : 1.f;

	
	return Params;
}

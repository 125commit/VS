// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/VS_WeaponAbility.h"
#include "Actor/VS_WeaponActor.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/VS_AttributeSet.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
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
	if (!GetAvatarActorFromActorInfo()) return;

	// 1. 获取四维属性 (从 AttributeSet 中读取最新的数值)
	float Cooldown = GetAttributeValue(UVS_AttributeSet::GetWeaponCooldownAttribute());
	float Area = GetAttributeValue(UVS_AttributeSet::GetWeaponAreaAttribute());
	float Duration = GetAttributeValue(UVS_AttributeSet::GetWeaponDurationAttribute());
	float Might = GetAttributeValue(UVS_AttributeSet::GetBaseDamageAttribute());

	// 容错：防止配置错误导致逻辑崩溃或无限死循环
	if (Cooldown <= 0.05f) Cooldown = 1.0f; // 冷却极限保护
	if (Area <= 0.01f) Area = 1.0f;         // 范围极限保护

	// 2. 生成实体武器打手
	if (WeaponActorClass)
	{
		FTransform SpawnTransform = GetAvatarActorFromActorInfo()->GetActorTransform();
		
		// 使用 Deferred 延迟生成，以便在 BeginPlay 之前灌入参数
		AVS_WeaponActor* Weapon = GetWorld()->SpawnActorDeferred<AVS_WeaponActor>(
			WeaponActorClass, 
			SpawnTransform, 
			GetAvatarActorFromActorInfo(), 
			Cast<APawn>(GetAvatarActorFromActorInfo()), 
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		if (Weapon)
		{
			// 3. 计算最终伤害
			float BaseDamage = 5.0f; // 底层兜底伤害
			if (GlobalAbilityInfoData)
			{
				FVSAbilityInfo Info = GlobalAbilityInfoData->FindAbilityInfoForTag(AbilityTag);
				int32 CurrentLevel = GetAbilityLevel();
				// 数组索引从0开始，CurrentLevel 从1开始
				if (Info.LevelDamages.IsValidIndex(CurrentLevel - 1))
				{
					BaseDamage = Info.LevelDamages[CurrentLevel - 1];
				}
			}
			
			float FinalDamage = BaseDamage * Might;

			// 注入核心参数：范围、存活时间、最终计算伤害
			Weapon->InitWeapon(Area, Duration, FinalDamage);

			// 完成生成 (会触发 Weapon 的 BeginPlay)
			Weapon->FinishSpawning(SpawnTransform);
		}
	}

	// 4. 进入大循环倒计时
	UAbilityTask_WaitDelay* DelayTask = UAbilityTask_WaitDelay::WaitDelay(this, Cooldown);
	DelayTask->OnFinish.AddDynamic(this, &UVS_WeaponAbility::OnWeaponFire);
	DelayTask->ReadyForActivation();
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/VS_AttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UVS_AttributeSet::UVS_AttributeSet()
{
	// 真正的初始数值将由 PlayerState 的 InitAbilityActorInfo 中应用 GameplayEffect 覆盖。
	// 这里的 0.f 仅作防止内存未初始化的底层兜底。
	InitHealth(0.f);
	InitMaxHealth(0.f);
	InitBaseDamage(0.f);          
	InitMagnetRadius(0.f);
	InitWeaponCooldown(0.f);
	InitWeaponDuration(0.f);
	InitWeaponArea(0.f);
	InitGreedMultiplier(0.f);
	
	// 元属性初始化
	InitIncomingDamage(0.f);
}
void UVS_AttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	// 注册属性 必须使用 COND_None 和 REPNOTIFY_Always，保证预测回滚和 UI 刷新强制执行！
	DOREPLIFETIME_CONDITION_NOTIFY(UVS_AttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVS_AttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVS_AttributeSet, BaseDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVS_AttributeSet, MagnetRadius, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVS_AttributeSet, WeaponCooldown, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVS_AttributeSet, WeaponDuration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVS_AttributeSet, WeaponArea, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UVS_AttributeSet, GreedMultiplier, COND_None, REPNOTIFY_Always);
}
// -------------------------------------------------------------
// OnRep 函数实现
// -------------------------------------------------------------
void UVS_AttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVS_AttributeSet, Health, OldHealth);
}
void UVS_AttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVS_AttributeSet, MaxHealth, OldMaxHealth);
}
void UVS_AttributeSet::OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVS_AttributeSet, BaseDamage, OldBaseDamage);
}
void UVS_AttributeSet::OnRep_MagnetRadius(const FGameplayAttributeData& OldMagnetRadius) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVS_AttributeSet, MagnetRadius, OldMagnetRadius);
}
void UVS_AttributeSet::OnRep_WeaponCooldown(const FGameplayAttributeData& OldWeaponCooldown) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVS_AttributeSet, WeaponCooldown, OldWeaponCooldown);
}
void UVS_AttributeSet::OnRep_WeaponDuration(const FGameplayAttributeData& OldWeaponDuration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVS_AttributeSet, WeaponDuration, OldWeaponDuration);
}
void UVS_AttributeSet::OnRep_WeaponArea(const FGameplayAttributeData& OldWeaponArea) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVS_AttributeSet, WeaponArea, OldWeaponArea);
}
void UVS_AttributeSet::OnRep_GreedMultiplier(const FGameplayAttributeData& OldGreedMultiplier) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UVS_AttributeSet, GreedMultiplier, OldGreedMultiplier);
}

// -------------------------------------------------------------
// 数值安全钳制与元属性结算
// -------------------------------------------------------------
void UVS_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	// 防线一：基础数值硬限制（如生命不可跌破 0）
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
}
void UVS_AttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	// 防线二：在所有的 GE 计算完成后执行最终钳制
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	// ==============================================================
	// 元属性消耗：处理受击伤害
	// ==============================================================
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalDamageDone = GetIncomingDamage();
		// 用完必须立刻清零！否则下一次会无限累加扣血
		SetIncomingDamage(0.f);
		if (LocalDamageDone > 0.f)
		{
			// 扣除真实生命值
			const float NewHealth = GetHealth() - LocalDamageDone;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));
			// 此处后续可发送受击标签、判定死亡等
		}
	}
	
}
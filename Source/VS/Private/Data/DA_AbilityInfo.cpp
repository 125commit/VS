#include "Data/DA_AbilityInfo.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/VS_AttributeSet.h"

FVSAbilityInfo UDA_AbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FVSAbilityInfo& Info : AbilityInformation)
	{
		// 精确匹配查表
		if (Info.AbilityTag.MatchesTagExact(AbilityTag))
		{
			return Info;
		}
	}

	// 如果没找到且允许报错，则打印红字警告
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("无法在 AbilityInfo [%s] 中找到 Tag 为 [%s] 的技能信息"), *GetNameSafe(this), *AbilityTag.ToString());
	}

	// 没找到则返回一个空的结构体
	return FVSAbilityInfo();
}

FVSAbilityLevelRow UDA_AbilityInfo::GetLevelRow(const FGameplayTag& AbilityTag, int32 Level, bool bLogNotFound) const
{
	FVSAbilityInfo Info = FindAbilityInfoForTag(AbilityTag, bLogNotFound);
	if (!Info.AbilityTag.IsValid())
	{
		return FVSAbilityLevelRow();
	}
	
	const int32 LevelIndex = Level -1;
	if (!Info.LevelRows.IsValidIndex(LevelIndex))
	{
		if (bLogNotFound)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ability [%s] Level [%d] 超出 LevelRows 范围 (Count=%d)"),
				*AbilityTag.ToString(), Level, Info.LevelRows.Num());
		}
		return FVSAbilityLevelRow();
	}
	
	return Info.LevelRows[LevelIndex];
}

float UDA_AbilityInfo::GetPassiveMagnitude(const FGameplayTag& AbilityTag, int32 Level, bool bLogNotFound) const
{
	return GetLevelRow(AbilityTag, Level, bLogNotFound).PassiveMagnitude;
}

float UDA_AbilityInfo::GetAttributeMultiplier(const UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, float DefaultValue)
{
	if (!ASC || !ASC->HasAttributeSetForAttribute(Attribute))
	{
		return DefaultValue;
	}
	return ASC->GetNumericAttribute(Attribute);
}

float UDA_AbilityInfo::SanitizeCooldown(float Cooldown)
{
	return Cooldown <= 0.05f ? 1.f : Cooldown;
}

float UDA_AbilityInfo::SanitizeArea(float Area)
{
	return Area <= 0.01f ? 1.f : Area;
}

FVSAbilityRuntimeStats UDA_AbilityInfo::ComputeAbilityStats(const FGameplayTag& AbilityTag, int32 AbilityLevel,
                                                            const UAbilitySystemComponent* ASC, bool bEvolvedNoCooldown, bool bEvolvedInfinitePierce) const
{
	const FVSAbilityLevelRow Row = GetLevelRow(AbilityTag, AbilityLevel, true);
	
	FVSAbilityRuntimeStats Stats; 
	Stats.BaseDamage = Row.BaseDamage;
	Stats.bNoCooldown = bEvolvedNoCooldown;
	
	const float CooldownMult = GetAttributeMultiplier(ASC, UVS_AttributeSet::GetWeaponCooldownAttribute(), 1.f);
	const float DurationMult = GetAttributeMultiplier(ASC, UVS_AttributeSet::GetWeaponDurationAttribute(), 1.f);
	const float AreaMult = GetAttributeMultiplier(ASC, UVS_AttributeSet::GetWeaponAreaAttribute(), 1.f);
	
	Stats.Cooldown        = SanitizeCooldown(Row.Cooldown * CooldownMult);
	Stats.Duration        = FMath::Max(0.f, Row.Duration * DurationMult);
	Stats.Area            = SanitizeArea(Row.Area * AreaMult);
	Stats.ProjectileCount = FMath::Max(1, Row.ProjectileCount);
	Stats.ProjectileSpeed = FMath::Max(0.f, Row.ProjectileSpeed);
	
	if (Stats.bNoCooldown)
	{
		Stats.Cooldown = 0.f;
	}
	return Stats;
}

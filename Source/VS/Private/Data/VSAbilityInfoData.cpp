#include "Data/VSAbilityInfoData.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/VS_AttributeSet.h"
#include "AbilitySystem/VS_AbilitySystemComponent.h"

FVSAbilityInfo UVSAbilityInfoData::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
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

FVSAbilityLevelRow UVSAbilityInfoData::GetLevelRow(const FGameplayTag& AbilityTag, int32 Level, bool bLogNotFound) const
{
	FVSAbilityInfo Info = FindAbilityInfoForTag(AbilityTag, bLogNotFound);
	if (!Info.AbilityTag.IsValid() || !Info.LevelTable)
	{
		return FVSAbilityLevelRow();
	}
	
	TArray<FVSAbilityLevelRow*> AllRows;
	Info.LevelTable->GetAllRows<FVSAbilityLevelRow>(TEXT("GetLevelRow"), AllRows);
	
	const int32 LevelIndex = Level - 1;
	if (!AllRows.IsValidIndex(LevelIndex) || !AllRows[LevelIndex])
	{
		if (bLogNotFound)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ability [%s] Level [%d] 超出 LevelTable 范围 (Count=%d)"),
				*AbilityTag.ToString(), Level, AllRows.Num());
		}
		return FVSAbilityLevelRow();
	}
	
	return *AllRows[LevelIndex];
}

float UVSAbilityInfoData::GetAttributeMultiplier(const UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, float DefaultValue)
{
	if (!ASC || !ASC->HasAttributeSetForAttribute(Attribute))
	{
		return DefaultValue;
	}
	return ASC->GetNumericAttribute(Attribute);
}

float UVSAbilityInfoData::SanitizeCooldown(float Cooldown)
{
	return Cooldown <= 0.05f ? 1.f : Cooldown;
}

float UVSAbilityInfoData::SanitizeArea(float Area)
{
	return Area <= 0.01f ? 1.f : Area;
}

FVSAbilityRuntimeStats UVSAbilityInfoData::ComputeAbilityStats(const FGameplayTag& AbilityTag, int32 AbilityLevel,
                                                            const UAbilitySystemComponent* ASC, bool bEvolvedNoCooldown, bool bEvolvedInfinitePierce) const
{
	FVSAbilityRuntimeStats Stats; 
	FVSAbilityInfo Info = FindAbilityInfoForTag(AbilityTag, true);

	// 如果是进化武器，直接去全局大表查数据
	if (Info.bIsEvolvedWeapon)
	{
		if (GlobalEvolvedWeaponStatsTable)
		{
			FVSEvolvedWeaponStatsRow* EvolvedRow = GlobalEvolvedWeaponStatsTable->FindRow<FVSEvolvedWeaponStatsRow>(AbilityTag.GetTagName(), TEXT("ComputeAbilityStats"));
			if (EvolvedRow)
			{
				Stats.BaseDamage = EvolvedRow->BaseDamage;
				Stats.bNoCooldown = EvolvedRow->bNoCooldown;
				Stats.Cooldown = SanitizeCooldown(EvolvedRow->Cooldown);
				Stats.Duration = FMath::Max(0.f, EvolvedRow->Duration);
				Stats.Area = SanitizeArea(EvolvedRow->Area);
				Stats.ProjectileCount = FMath::Max(1, EvolvedRow->ProjectileCount);
				Stats.ProjectileSpeed = FMath::Max(0.f, EvolvedRow->ProjectileSpeed);
				Stats.SpeedMultiplier = FMath::Max(0.f, EvolvedRow->SpeedMultiplier);
				Stats.PierceCount = EvolvedRow->PierceCount;

				if (Stats.bNoCooldown)
				{
					Stats.Cooldown = 0.15f;
				}
				// 进化武器通常不受普通 CD 缩减影响，或者根据策划需求自行乘 Mult
				return Stats;
			}
		}
		UE_LOG(LogTemp, Error, TEXT("无法在全局大表 GlobalEvolvedWeaponStatsTable 中找到进化武器 [%s] 的配方!"), *AbilityTag.ToString());
		return Stats;
	}

	// 普通武器逻辑
	const FVSAbilityLevelRow Row = GetLevelRow(AbilityTag, AbilityLevel, true);
	
	Stats.BaseDamage = Row.BaseDamage;
	Stats.bNoCooldown = bEvolvedNoCooldown;
	
	const float CooldownMult = GetAttributeMultiplier(ASC, UVS_AttributeSet::GetCooldownMultAttribute(), 1.f);
	const float DurationMult = GetAttributeMultiplier(ASC, UVS_AttributeSet::GetDurationMultAttribute(), 1.f);
	const float AreaMult = GetAttributeMultiplier(ASC, UVS_AttributeSet::GetAreaMultAttribute(), 1.f);
	
	Stats.Cooldown        = SanitizeCooldown(Row.Cooldown * CooldownMult);
	Stats.Duration        = FMath::Max(0.f, Row.Duration * DurationMult);
	Stats.Area            = SanitizeArea(Row.Area * AreaMult);
	Stats.ProjectileCount = FMath::Max(1, Row.ProjectileCount);
	Stats.ProjectileSpeed = FMath::Max(0.f, Row.ProjectileSpeed);
	Stats.SpeedMultiplier = FMath::Max(0.f, Row.SpeedMultiplier);
	
	if (Stats.bNoCooldown)
	{
		Stats.Cooldown = 0.15f;
	}
	return Stats;
}

FGameplayTag UVSAbilityInfoData::CheckIfCausesEvolution(const FGameplayTag& CandidateTag, int32 CandidatePredictedLevel, const UAbilitySystemComponent* ASC) const
{
	if (!ASC || !GlobalEvolvedWeaponStatsTable)
	{
		return FGameplayTag::EmptyTag;
	}

	const UVS_AbilitySystemComponent* VSASC = Cast<const UVS_AbilitySystemComponent>(ASC);
	if (!VSASC)
	{
		return FGameplayTag::EmptyTag;
	}

	TArray<FVSEvolvedWeaponStatsRow*> AllEvolutionRows;
	GlobalEvolvedWeaponStatsTable->GetAllRows<FVSEvolvedWeaponStatsRow>(TEXT("CheckIfCausesEvolution"), AllEvolutionRows);

	TArray<FVSOwnedAbilityInfo> OwnedAbilities = VSASC->GetOwnedAbilities();

	// 遍历所有进化配方
	for (const FVSEvolvedWeaponStatsRow* Row : AllEvolutionRows)
	{
		if (!Row) continue;

		// 情况 1：发牌是武器，尝试补全这个配方
		if (Row->RequiredWeaponTag.MatchesTagExact(CandidateTag))
		{
			FVSAbilityInfo WeaponInfo = FindAbilityInfoForTag(Row->RequiredWeaponTag);
			bool bWeaponLevelOk = Row->bWeaponNeedsMaxLevel ? (CandidatePredictedLevel >= WeaponInfo.MaxLevel) : true;

			if (bWeaponLevelOk)
			{
				// 检查玩家身上有没有该配方需要的被动
				bool bHasRequiredPassive = false;
				for (const FVSOwnedAbilityInfo& Owned : OwnedAbilities)
				{
					if (Owned.AbilityTag.MatchesTagExact(Row->RequiredPassiveTag))
					{
						FVSAbilityInfo PassiveInfo = FindAbilityInfoForTag(Row->RequiredPassiveTag);
						bool bPassiveLevelOk = Row->bPassiveNeedsMaxLevel ? (Owned.Level >= PassiveInfo.MaxLevel) : true;
						if (bPassiveLevelOk)
						{
							bHasRequiredPassive = true;
							break;
						}
					}
				}

				if (bHasRequiredPassive)
				{
					// 配方成立！通过 RowName (通常也就是 EvolvedWeapon 的 Tag) 构造 Tag 并返回
					const TMap<FName, uint8*>& RowMap = GlobalEvolvedWeaponStatsTable->GetRowMap();
					for (auto& Pair : RowMap)
					{
						if (Pair.Value == reinterpret_cast<uint8*>(const_cast<FVSEvolvedWeaponStatsRow*>(Row)))
						{
							return FGameplayTag::RequestGameplayTag(Pair.Key);
						}
					}
				}
			}
		}
		// 情况 2：发牌是被动，尝试补全这个配方
		else if (Row->RequiredPassiveTag.MatchesTagExact(CandidateTag))
		{
			FVSAbilityInfo PassiveInfo = FindAbilityInfoForTag(Row->RequiredPassiveTag);
			bool bPassiveLevelOk = Row->bPassiveNeedsMaxLevel ? (CandidatePredictedLevel >= PassiveInfo.MaxLevel) : true;

			if (bPassiveLevelOk)
			{
				// 检查玩家身上有没有该配方需要的武器
				bool bHasRequiredWeapon = false;
				for (const FVSOwnedAbilityInfo& Owned : OwnedAbilities)
				{
					if (Owned.AbilityTag.MatchesTagExact(Row->RequiredWeaponTag))
					{
						FVSAbilityInfo WeaponInfo = FindAbilityInfoForTag(Row->RequiredWeaponTag);
						bool bWeaponLevelOk = Row->bWeaponNeedsMaxLevel ? (Owned.Level >= WeaponInfo.MaxLevel) : true;
						if (bWeaponLevelOk)
						{
							bHasRequiredWeapon = true;
							break;
						}
					}
				}

				if (bHasRequiredWeapon)
				{
					// 配方成立！返回对应的进化后武器 Tag
					const TMap<FName, uint8*>& RowMap = GlobalEvolvedWeaponStatsTable->GetRowMap();
					for (auto& Pair : RowMap)
					{
						if (Pair.Value == reinterpret_cast<uint8*>(const_cast<FVSEvolvedWeaponStatsRow*>(Row)))
						{
							return FGameplayTag::RequestGameplayTag(Pair.Key);
						}
					}
				}
			}
		}
	}

	return FGameplayTag::EmptyTag;
}

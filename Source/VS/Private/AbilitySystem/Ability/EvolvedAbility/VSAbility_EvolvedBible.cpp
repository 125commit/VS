// LAvid


#include "AbilitySystem/Ability/EvolvedAbility/VSAbility_EvolvedBible.h"

#include "VSGameplayTags.h"
#include "Data/VSAbilityInfoData.h"

UVSAbility_EvolvedBible::UVSAbility_EvolvedBible()
{
	SetupWeaponAbility(FVSGameplayTags::Get().Abilities_Weapon_EvolvedBook);
}

float UVSAbility_EvolvedBible::GetNextFireDelay(const FVSAbilityRuntimeStats& Stats) const
{
	// 父类在 bNoCooldown 时返回 0 → 每帧重复生成书会刷爆对象池。
	// 渎神祷书要"一直在场旋转"：下一轮 = 当前书的在场时长，到点无缝换一批。
	// 配合进化配方表把 Duration 配得极大（如 99999），即可视为永久常驻。
	return Stats.Duration > 0.f ? Stats.Duration : 3.f;
}

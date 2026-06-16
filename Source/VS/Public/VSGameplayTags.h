#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * 包含了全游戏所有核心标签的原生 C++ 结构体单例
 */
struct FVSGameplayTags
{
public:
	static const FVSGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeGameplayTags();

	// ==========================================
	// 技能类型 (用于划分槽位)
	// ==========================================
	FGameplayTag Abilities_Type_Weapon;  // 武器槽标记
	FGameplayTag Abilities_Type_Passive; // 被动槽标记

	// ==========================================
	// 具体技能身份 (用于抽卡查表与 ASC 校验)
	// ==========================================
	
	// --- 武器 (Weapons) ---
	FGameplayTag Abilities_Weapon_Whip;      // 神鞭
	FGameplayTag Abilities_Weapon_Garlic;    // 大蒜
	FGameplayTag Abilities_Weapon_MagicBook; // 魔法书
	FGameplayTag Abilities_Weapon_FireWand;  // 火焰魔杖
	FGameplayTag Abilities_Weapon_MagicWand; // 强能魔杖

	// --- 被动 (Passives) ---
	FGameplayTag Abilities_Passive_Pummarola;   // 愈伤番茄
	FGameplayTag Abilities_Passive_HollowHeart; // 虚空之心
	FGameplayTag Abilities_Passive_Spellbinder; // 咒缚盒
	FGameplayTag Abilities_Passive_EmptyTome;   // 空白之书
	FGameplayTag Abilities_Passive_Spinach;     // 菠菜
	
protected:
	static FVSGameplayTags GameplayTags;
};
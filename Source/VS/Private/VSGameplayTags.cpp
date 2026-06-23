#include "VSGameplayTags.h"
#include "GameplayTagsManager.h"

FVSGameplayTags FVSGameplayTags::GameplayTags;

void FVSGameplayTags::InitializeNativeGameplayTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();

	// -------------------------------------------------------------
	// 注册：技能类型 Tags
	// -------------------------------------------------------------
	GameplayTags.Abilities_Type_Weapon = Manager.AddNativeGameplayTag(
		FName("Abilities.Type.Weapon"),
		FString("代表这是一个武器类型的技能，占用武器槽位")
	);

	GameplayTags.Abilities_Type_Passive = Manager.AddNativeGameplayTag(
		FName("Abilities.Type.Passive"),
		FString("代表这是一个被动类型的技能，占用被动槽位")
	);

	// -------------------------------------------------------------
	// 注册：武器 Tags
	// -------------------------------------------------------------
	GameplayTags.Abilities_Weapon_Whip = Manager.AddNativeGameplayTag(
		FName("Abilities.Weapon.Whip"),
		FString("武器：神鞭")
	);

	GameplayTags.Abilities_Weapon_Garlic = Manager.AddNativeGameplayTag(
		FName("Abilities.Weapon.Garlic"),
		FString("武器：大蒜")
	);

	GameplayTags.Abilities_Weapon_MagicBook = Manager.AddNativeGameplayTag(
		FName("Abilities.Weapon.MagicBook"),
		FString("武器：魔法书")
	);

	GameplayTags.Abilities_Weapon_FireWand = Manager.AddNativeGameplayTag(
		FName("Abilities.Weapon.FireWand"),
		FString("武器：火焰魔杖")
	);

	GameplayTags.Abilities_Weapon_MagicWand = Manager.AddNativeGameplayTag(
		FName("Abilities.Weapon.MagicWand"),
		FString("武器：强能魔杖")
	);

	// -- 进化武器 -- //
	GameplayTags.Abilities_Weapon_EvolvedWhip = Manager.AddNativeGameplayTag(
		FName("Abilities.Weapon.EvolvedWhip"),
		FString("进化武器：吸血鞭")
		);
	
	GameplayTags.Abilities_Weapon_EvolvedBook = Manager.AddNativeGameplayTag(
		FName("Abilities.Weapon.EvolvedBook"),
		FString("进化武器：渎神祷书")
		);
		
	GameplayTags.Abilities_Weapon_EvolvedGarlic = Manager.AddNativeGameplayTag(
		FName("Abilities.Weapon.EvolvedGarlic"),
		FString("进化武器：噬魂纹章")
		);
	
	GameplayTags.Abilities_Weapon_EvolvedMagicWand = Manager.AddNativeGameplayTag(
		FName("Abilities.Weapon.EvolvedMagicWand"),
		FString("进化武器：圣魔杖")
		);	
		
	GameplayTags.Abilities_Weapon_EvolvedFireWand = Manager.AddNativeGameplayTag(
		FName("Abilities.Weapon.EvolvedFireWand"),
		FString("进化武器：地狱火")
		);
	
	
	// -------------------------------------------------------------
	// 注册：被动 Tags
	// -------------------------------------------------------------
	GameplayTags.Abilities_Passive_Pummarola = Manager.AddNativeGameplayTag(
		FName("Abilities.Passive.Pummarola"),
		FString("被动：愈伤番茄 (回血加成)")
	);

	GameplayTags.Abilities_Passive_HollowHeart = Manager.AddNativeGameplayTag(
		FName("Abilities.Passive.HollowHeart"),
		FString("被动：虚空之心 (最大生命加成)")
	);

	GameplayTags.Abilities_Passive_Spellbinder = Manager.AddNativeGameplayTag(
		FName("Abilities.Passive.Spellbinder"),
		FString("被动：咒缚盒 (持续时间加成)")
	);

	GameplayTags.Abilities_Passive_EmptyTome = Manager.AddNativeGameplayTag(
		FName("Abilities.Passive.EmptyTome"),
		FString("被动：空白之书 (冷却缩减)")
	);

	GameplayTags.Abilities_Passive_Spinach = Manager.AddNativeGameplayTag(
		FName("Abilities.Passive.Spinach"),
		FString("被动：菠菜 (伤害加成)")
	);
	
	
	
	GameplayTags.Data_Passive_Magnitude = Manager.AddNativeGameplayTag(
		FName("Data.Passive.Magnitude"), 
		FString("被动 SetByCaller 数值通道"));
	
	GameplayTags.Ability_Upgrade = Manager.AddNativeGameplayTag(
		FName("Ability.Upgrade"), 
		FString("升级标签"));
	
}
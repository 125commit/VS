#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DA_AbilityInfo.generated.h"

class UTexture2D;
class UMaterialInterface;
class UGameplayAbility; // 或者替换为您的武器基类 UVS_WeaponAbilityBase

// ==========================================================
// 核心数据结构：完全对应单张卡牌的所有信息
// ==========================================================
USTRUCT(BlueprintType)
struct FVSAbilityInfo
{
	GENERATED_BODY()

	// 1. 技能独一无二的身份 Tag (例如: Abilities.Weapon.Garlic)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInfo")
	FGameplayTag AbilityTag;

	// 2. 类别 Tag：区分是被动还是武器 (例如: Abilities.Type.Weapon, Abilities.Type.Passive)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInfo")
	FGameplayTag AbilityTypeTag;

	// 3. 后台硬核数据：服务器实际拿来发给玩家的技能类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInfo")
	TSubclassOf<UGameplayAbility> AbilityClass;

	// ------------------------------------------------------
	// UI 富文本表现数据 (传给 WidgetController 用的)
	// ------------------------------------------------------

	// UI 展示：技能名称 ("大蒜")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInfo|UI")
	FText Name;

	// UI 展示：每一级的文字描述！
	// 索引0=1级("对周围造成伤害"), 索引1=2级("范围+20%,伤害+5")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInfo|UI")
	TArray<FText> LevelDescriptions; 

	// UI 展示：技能图标
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInfo|UI")
	TObjectPtr<const UTexture2D> Icon;

	// UI 展示：技能背景图 (比如金色背景代表被动，红色代表武器)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInfo|UI")
	TObjectPtr<const UMaterialInterface> BackgroundMaterial;

	// ------------------------------------------------------
	// 满级上限
	// ------------------------------------------------------

	// 满级上限统一设定为 5 级，减轻战斗逻辑开发量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInfo|Rules")
	int32 MaxLevel = 5;

	// UI/Logic：每一级的基础伤害数值
	// 索引0=1级的基础伤害，索引1=2级的基础伤害...
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInfo|Rules")
	TArray<float> LevelDamages;
};

// ==========================================================
// 资产大字典类
// ==========================================================
UCLASS()
class VS_API UDA_AbilityInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	// 策划在这里配置全游戏所有的技能
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInformation")
	TArray<FVSAbilityInfo> AbilityInformation;

	// ==========================================================
	// 辅助函数：根据 Tag 查表
	// (在 PC 抽卡以及 Client_ShowLevelUpScreen 中高频调用！)
	// ==========================================================
	UFUNCTION(BlueprintCallable, Category = "AbilityInformation")
	FVSAbilityInfo FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound = false) const;
};
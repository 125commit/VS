#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DA_AbilityInfo.generated.h"

class UTexture2D;
class UMaterialInterface;
struct FGameplayAttribute;
class UAbilitySystemComponent;
class UGameplayAbility; // 或者替换为您的武器基类 UVS_WeaponAbilityBase


// ==========================================================
// 技能的基础属性
// ==========================================================
USTRUCT(BlueprintType)
struct FVSAbilityLevelRow
{
	GENERATED_BODY()
	
	//默认的基础伤害值，具体的在ExecCal中计算
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelRow")
	float BaseDamage = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelRow")
	float Cooldown = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelRow")
	float Duration = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelRow")
	float Area = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelRow")
	int32 ProjectileCount = 1;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelRow")
	int32 PierceCount = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelRow")
	float ProjectileSpeed = 600.f;
	
	// 被动：ApplyPassiveEffect 通过 SetByCaller 传给 GE 的数值
	// （如菠菜伤害加成、空心之心加血等，具体语义由被动 GE 决定）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelRow")
	float PassiveMagnitude = 0.f;
};

// ==========================================================
// 运行时计算并修改后的属性值（ C++用 ）
// ==========================================================
USTRUCT(BlueprintType)
struct FVSAbilityRuntimeStats
{
	GENERATED_BODY()
	// 表值，不在此乘 Might；最终伤害由 ExecutionCalculation 处理
	UPROPERTY(BlueprintReadOnly, Category = "WeaponStats")
	float BaseDamage = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "WeaponStats")
	float Cooldown = 1.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "WeaponStats")
	float Duration = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "WeaponStats")
	float Area = 1.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "WeaponStats")
	int32 ProjectileCount = 1;
	
	UPROPERTY(BlueprintReadOnly, Category = "WeaponStats")
	float ProjectileSpeed = 600.f;
	
	// 进化：无 CD 连发
	UPROPERTY(BlueprintReadOnly, Category = "WeaponStats")
	bool bNoCooldown = false;
};

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInfo|Rules")
	TArray<FVSAbilityLevelRow> LevelRows;
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
	
	UFUNCTION(BlueprintCallable, Category = "AbilityInformation")
	FVSAbilityLevelRow GetLevelRow(const FGameplayTag& AbilityTag, int32 Level, bool bLogNotFound = false) const;
	
	UFUNCTION(BlueprintCallable, Category = "AbilityInformation")
	float GetPassiveMagnitude(const FGameplayTag& AbilityTag, int32 Level, bool bLogNotFound = false) const;

	//计算技能的基础属性
	UFUNCTION(BlueprintCallable, Category = "AbilityInformation")
	FVSAbilityRuntimeStats ComputeAbilityStats(
		const FGameplayTag& AbilityTag,
		int32 AbilityLevel,
		const UAbilitySystemComponent* ASC,
		bool bEvolvedNoCooldown = false,
		bool bEvolvedInfinitePierce = false) const;
	
	
private:
	static float GetAttributeMultiplier(const UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute, float DefaultValue);
	static float SanitizeCooldown(float Cooldown);
	static float SanitizeArea(float Area);
};
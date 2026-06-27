#pragma once

#include "CoreMinimal.h" 
#include "Engine/DataAsset.h"
#include "DA_CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class USkeletalMesh;

// -----------------------------------------------------------------
// 1. 英雄图鉴枚举 (不包含怪物)
// -----------------------------------------------------------------
UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Antonio,    // 对应初始武器：神鞭 (Whip)
	Poe,        // 对应初始武器：大蒜 (Garlic)
	Clerici,    // 对应初始武器：魔法书 (King Bible)
	Arca,       // 对应初始武器：火焰魔杖 (Fire Wand)
	Imelda      // 对应初始武器：强能魔杖 (Magic Wand)
};

// -----------------------------------------------------------------
// 2. 单个英雄的核心配置包
// -----------------------------------------------------------------
USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	// 1. 该角色的专属骨骼模型 (UI 选人后，GameMode 动态替换玩家 Mesh)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class Defaults")
	TObjectPtr<USkeletalMesh> CharacterMesh;

	// NOTE: EditAnywhere 便于编辑器脚本批量写入各角色初始武器
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	// 3. 初始属性加成 (使用 GameplayEffect 承载，如：Antonio 血量上限+20，护甲+1)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;
};

// -----------------------------------------------------------------
// 3. 全局数据资产大表
// -----------------------------------------------------------------
UCLASS()
class VS_API UDA_CharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	// O(1) 复杂度的字典：通过枚举直接查找英雄配置
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;

	// 工具函数：安全查表
	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass);
};
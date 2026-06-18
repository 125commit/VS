#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DA_ToolInfo.generated.h"

/**
 * 元进度/商店道具的单条配置信息
 */
USTRUCT(BlueprintType)
struct FMetaItemInfo
{
	GENERATED_BODY()

	// 每一级的购买价格。数组的长度代表了该物品的最大等级。
	// 例如：[500, 1000] 代表共两级。升1级花费500，升2级花费1000。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Store")
	TArray<int32> LevelPrices;

	// 每一级提供的加成数值。
	// 例如：[0.05, 0.10] 代表1级提供 5% 加成，2级提供 10% 加成。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Store")
	TArray<float> LevelBonuses;

	// 局内应用时，通过 SetByCaller 注入 GameplayEffect 时使用的标识符。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Store")
	FGameplayTag SetByCallerTag;
};

/**
 * 全局元进度/商店定价与加成表
 */
UCLASS()
class VS_API UDA_ToolInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	// 字典：Tag -> 多级配置信息
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tool Info")
	TMap<FGameplayTag, FMetaItemInfo> MetaItems;

	/**
	 * 工具函数：输入一个 Tag，返回对应物品信息指针。
	 * 如果没有配置，返回 nullptr。
	 */
	const FMetaItemInfo* GetMetaItemInfo(FGameplayTag ItemTag) const
	{
		return MetaItems.Find(ItemTag);
	}
};
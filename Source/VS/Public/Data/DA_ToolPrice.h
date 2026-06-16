#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DA_ToolPrice.generated.h"

/**
 * 道具定价表。策划在编辑器创建这个资产后，可以直接添加数组元素：
 * Tag: Abilities.Passive.Pummarola -> Price: 500
 */
UCLASS()
class VS_API UDA_ToolPrice : public UDataAsset
{
	GENERATED_BODY()

public:

	// 使用 TMap 字典实现完美的 Tag -> Price 映射
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tool Prices")
	TMap<FGameplayTag, int32> ItemPrices;

	/**
	 * 工具函数：输入一个 Tag，返回对应金币价格。
	 * 如果没有配置，返回 -1 表示不可购买。
	 */
	int32 GetPriceForTag(FGameplayTag ItemTag) const
	{
		if (const int32* FoundPrice = ItemPrices.Find(ItemTag))
		{
			return *FoundPrice;
		}
		return -1;
	}
};
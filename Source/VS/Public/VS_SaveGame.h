#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "VS_SaveGame.generated.h"

/**
 * 这是虚幻引擎底层的存盘“塑料袋”。
 * 当写入存档时，引擎会自动把这个类里标记了 UPROPERTY 的变量转化为二进制字节流。
 */
UCLASS()
class VS_API UVS_SaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// 唯一指定存档槽名称
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	FString SaveSlotName = TEXT("VSSaveSlot_01");

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	int32 UserIndex = 0;

	// ==========================================
	// 真正需要落盘的永久数据
	// ==========================================
	
	// 账户总金币
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	int32 SavedTotalGold = 0;

	// 已解锁的道具/角色等级字典：Tag -> 当前等级 (从 1 开始)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "SaveData")
	TMap<FGameplayTag, int32> SavedItemLevels;
	
	// 您可以随时在这里补充：比如已解锁角色、局外天赋等级字典等
};
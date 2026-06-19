#include "VS_GameInstance.h"
#include "VS_SaveGame.h"
#include "Kismet/GameplayStatics.h"

void UVS_GameInstance::Init()
{
	Super::Init();
	
	// 游戏一启动，立刻把硬盘数据提取到内存里
	LoadDataFromDisk();
}

bool UVS_GameInstance::SpendGold(int32 Cost)
{
	if (TotalGold >= Cost)
	{
		TotalGold -= Cost;
		OnGoldChangedDelegate.Broadcast(TotalGold); // 广播给所有 UI
		SaveDataToDisk(); // 内存改变，立刻落盘！
		return true;
	}
	return false;
}

void UVS_GameInstance::AddTotalGold(int32 Amount)
{
	if (Amount > 0)
	{
		TotalGold += Amount;
		OnGoldChangedDelegate.Broadcast(TotalGold); // 广播给所有 UI
		SaveDataToDisk(); // 内存改变，立刻落盘！
	}
}

void UVS_GameInstance::UpgradeItemLevel(FGameplayTag ItemTag)
{
	int32 CurrentLevel = GetItemLevel(ItemTag);
	int32 NewLevel = CurrentLevel + 1;
	
	ItemLevels.Add(ItemTag, NewLevel);
	OnItemLevelChangedDelegate.Broadcast(ItemTag, NewLevel); // 广播通知商店界面
	SaveDataToDisk(); // 新物品落盘
}

int32 UVS_GameInstance::GetItemLevel(FGameplayTag ItemTag) const
{
	if (const int32* FoundLevel = ItemLevels.Find(ItemTag))
	{
		return *FoundLevel;
	}
	return 0; // 0代表尚未解锁
}

void UVS_GameInstance::SetLastMatchResult(const FVSMatchResult& InResult)
{
	LastMatchResult = InResult;
	// 战局快照属于瞬时数据，UI 读取完就可以作废，不需要落盘
}

void UVS_GameInstance::LoadDataFromDisk()
{
	if (UGameplayStatics::DoesSaveGameExist(DEFAULT_SLOT_NAME, 0))
	{
		// 读档
		USaveGame* LoadedGame = UGameplayStatics::LoadGameFromSlot(DEFAULT_SLOT_NAME, 0);
		if (UVS_SaveGame* VSSave = Cast<UVS_SaveGame>(LoadedGame))
		{
			// 将存档数据灌入 GameInstance 内存
			TotalGold = VSSave->SavedTotalGold;
			ItemLevels = VSSave->SavedItemLevels; // 读取已拥有的物品等级
			UE_LOG(LogTemp, Log, TEXT("读取存档成功！当前金币: %d，已解锁物品种类: %d"), TotalGold, ItemLevels.Num());
			return;
		}
	}

	// 没找到存档说明是第一次玩，默认值生效即可
	UE_LOG(LogTemp, Log, TEXT("未找到存档，创建全新的开始。"));
}

void UVS_GameInstance::SaveDataToDisk()
{
	// 创建新的存档空壳
	USaveGame* SaveGameObj = UGameplayStatics::CreateSaveGameObject(UVS_SaveGame::StaticClass());
	if (UVS_SaveGame* VSSave = Cast<UVS_SaveGame>(SaveGameObj))
	{
		// 把内存数据倒进空壳
		VSSave->SavedTotalGold = this->TotalGold;
		VSSave->SavedItemLevels = this->ItemLevels; // 保存已拥有的物品等级

		// 落盘
		UGameplayStatics::SaveGameToSlot(VSSave, DEFAULT_SLOT_NAME, 0);
		UE_LOG(LogTemp, Log, TEXT("数据已自动落盘，最新金币: %d，已解锁物品: %d 个"), TotalGold, ItemLevels.Num());
	}
}
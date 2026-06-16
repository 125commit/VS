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
		SaveDataToDisk(); // 内存改变，立刻落盘！
	}
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
			UE_LOG(LogTemp, Log, TEXT("读取存档成功！当前金币: %d"), TotalGold);
			return;
		}
	}

	// 没找到存档说明是第一次玩，金币默认是 0，不用做处理
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

		// 落盘
		UGameplayStatics::SaveGameToSlot(VSSave, DEFAULT_SLOT_NAME, 0);
		UE_LOG(LogTemp, Log, TEXT("数据已自动落盘，最新金币: %d"), TotalGold);
	}
}
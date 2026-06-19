#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AbilitySystem/VS_AbilitySystemComponent.h" // 包含 FVSOwnedAbilityInfo
#include "VS_GameInstance.generated.h"

// ==========================================================
// 结构体：暂存单局战果 (用于结算屏幕 UI 读取)
// ==========================================================
USTRUCT(BlueprintType)
struct FVSMatchResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Match Result")
	int32 MatchLevel = 1;

	UPROPERTY(BlueprintReadOnly, Category = "Match Result")
	float MatchXP = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "Match Result")
	int32 MatchKillCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Match Result")
	int32 MatchGold = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Match Result")
	TArray<FVSOwnedAbilityInfo> MatchAbilities; // 局内抽到的所有技能与等级
};


// ==========================================================
// 委托：用于通知全生命周期的系统（如局外商店UI）金币或物品变化
// ==========================================================
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGlobalGoldChangedDelegate, int32, NewGold);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemLevelChangedDelegate, FGameplayTag, ItemTag, int32, NewLevel);

UCLASS()
class VS_API UVS_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// 引擎启动时自动执行，是执行 LoadGame 的完美时机
	virtual void Init() override;

	// ==========================================================
	// 内存活跃变量
	// ==========================================================
	
	// 当前账户总资产
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VS|Account")
	int32 TotalGold = 0;

	// 局外永久道具等级字典：Tag -> 当前等级
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VS|Account")
	TMap<FGameplayTag, int32> ItemLevels;

	// 最近一次战斗的战利品（切换关卡时存放在这里不死，结算 UI 随用随取）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VS|Match")
	FVSMatchResult LastMatchResult;

	// ==========================================================
	// 全局广播委托
	// ==========================================================
	
	UPROPERTY(BlueprintAssignable, Category = "VS|Delegates")
	FOnGlobalGoldChangedDelegate OnGoldChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "VS|Delegates")
	FOnItemLevelChangedDelegate OnItemLevelChangedDelegate;

	// ==========================================================
	// 给 GM / PC 调用的傻瓜式接口 (自带自动落盘机制)
	// ==========================================================
	
	// 购买物品扣费，返回 true 即为余额充足且扣款成功
	UFUNCTION(BlueprintCallable, Category = "VS|Account")
	bool SpendGold(int32 Cost);

	// 战斗结算加钱
	UFUNCTION(BlueprintCallable, Category = "VS|Account")
	void AddTotalGold(int32 Amount);

	// 升级/解锁物品并落盘
	UFUNCTION(BlueprintCallable, Category = "VS|Account")
	void UpgradeItemLevel(FGameplayTag ItemTag);

	// 获取物品当前等级（0 代表未解锁）
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VS|Account")
	int32 GetItemLevel(FGameplayTag ItemTag) const;

	// 设置战利品快照
	UFUNCTION(BlueprintCallable, Category = "VS|Match")
	void SetLastMatchResult(const FVSMatchResult& InResult);

private:
	// ==========================================================
	// 内部私有的硬盘 I/O 操作
	// ==========================================================
	void LoadDataFromDisk();
	void SaveDataToDisk();

	const FString DEFAULT_SLOT_NAME = TEXT("VSSaveSlot_01");
};
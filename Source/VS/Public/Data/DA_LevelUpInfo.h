#pragma once


#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_LevelUpInfo.generated.h"

// 每一级的升级数据结构体
USTRUCT(BlueprintType)
struct FVS_LevelUpData
{
	GENERATED_BODY()

	// 升到当前级别所需要的【总经验值】起跑线
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float XPRequirement = 0.f;
};

/**
 * 专门供策划配置的升级经验表
 */
UCLASS()
class VS_API UDA_LevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	// 数组下标即为 Level。例如：Index 1 代表第 1 级的要求（通常是 0），Index 2 代表第 2 级。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUpInformation")
	TArray<FVS_LevelUpData> LevelUpInformation;

	// 查表：当前 XP 究竟能满足到多少级？
	int32 FindLevelForXP(float XP) const;

	// 查表：获取特定等级的起跑线 XP
	float GetXPRequirementForLevel(int32 Level) const;
};
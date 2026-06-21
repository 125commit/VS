#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "VSLevelUpData.generated.h"

// 每一级的升级数据结构体
USTRUCT(BlueprintType)
struct FVS_LevelUpData : public FTableRowBase
{
	GENERATED_BODY()

	// 升到当前级别所需要的【总经验值】起跑线
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float XPRequirement = 0.f;
};
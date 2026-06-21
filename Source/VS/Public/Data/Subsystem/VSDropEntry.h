// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Actor/VSDropItem.h"
#include "VSDropEntry.generated.h"

USTRUCT(BlueprintType)
struct FDropEntry : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	EVSDropType DropType = EVSDropType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	float DropValue = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	bool bGuaranteed = false;

	// 通用掉落概率 0~1，仅当 bGuaranteed=false 时生效
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop", meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "!bGuaranteed"))
	float DropChance = 1.f;
};

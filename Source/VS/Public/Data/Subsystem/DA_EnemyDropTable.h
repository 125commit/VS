// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Actor/VSDropItem.h"
#include "DA_EnemyDropTable.generated.h"

USTRUCT(BlueprintType)
struct FDropEntry
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

UCLASS()
class VS_API UDA_EnemyDropTable : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Drop")
	TArray<FDropEntry> DropEntries;

	bool IsDrop(FDropEntry& OutEntry) const;
};

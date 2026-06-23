// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "VSDropData.generated.h"

class AVS_DropItemActor;
class UTexture2D;

UENUM(BlueprintType)
enum class EVSDropType : uint8
{
	None,
	XPSmall,
	XPLarge, 
	Gold,
	Turkey,
	Box,
};

USTRUCT(BlueprintType)
struct FVSDropEntry : public FTableRowBase
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

//一个 Type 对应一个 蓝图Class
USTRUCT(BlueprintType)
struct FVSDropTypeDefinition
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AVS_DropItemActor> DropActorClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 PreSpawnCount = 20;
	
	// UI 可用
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon;
};

UCLASS()
class VS_API UVS_DropItemDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EVSDropType, FVSDropTypeDefinition> DropTypeToTypeDef;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefaultMagnetRadius = 100.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MagnetMoveSpeed = 500.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PickupRadius = 50.f;
	
	FVSDropTypeDefinition FindDefinition(EVSDropType Type) const;
	TSubclassOf<AVS_DropItemActor> FindDropClass(EVSDropType Type) const;
};

// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Actor/VSDropItem.h"
#include "DA_DropItems.generated.h"

class AVSDropItem;


//一个 Type 对应一个 蓝图Class
USTRUCT(BlueprintType)
struct FDropTypeDefinition
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AVSDropItem> DropActorClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 PreSpawnCount = 20;
	
	// UI 可用
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Icon;
};

/**
 * 
 */
UCLASS()
class VS_API UDA_DropItems : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EVSDropType, FDropTypeDefinition> DropTypeToTypeDef;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefaultMagnetRadius = 100.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MagnetMoveSpeed = 500.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PickupRadius = 50.f;
	
	FDropTypeDefinition FindDefinition(EVSDropType Type) const;
	TSubclassOf<AVSDropItem> FindDropClass(EVSDropType Type) const;
	
};

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Character/VSEnemyType.h"
#include "DA_EnemyDictionary.generated.h"

class AVSEnemy;

/**
 * 怪物字典：将枚举映射到实际的怪物蓝图类
 */
UCLASS()
class VS_API UDA_EnemyDictionary : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 策划在这里配置所有怪物类型的蓝图类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EnemyDictionary")
	TMap<EVSEnemyType, TSubclassOf<AVSEnemy>> EnemyClassMap;

	// 获取怪物类
	UFUNCTION(BlueprintCallable, Category = "EnemyDictionary")
	TSubclassOf<AVSEnemy> GetEnemyClass(EVSEnemyType Type) const;
};

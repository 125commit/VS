// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Character/VSEnemyType.h"
#include "Data/Subsystem/DA_EnemyDictionary.h"
#include "DA_WaveDirectorConfig.generated.h"

USTRUCT(BlueprintType)
struct FWaveSpawnConfig : public FTableRowBase
{
    GENERATED_BODY()
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EVSEnemyType EnemyType = EVSEnemyType::None;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float StartTime = 0.f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float EndTime = 300.f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float SpawnInterval = 8.f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 EnemyNumPerWave = 10;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bIsElite = false;
};

/**
 * 
 */
UCLASS()
class VS_API UDA_WaveDirectorConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 波次具体配置表
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UDataTable> WavesTable;

	// 怪物种类字典
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UDA_EnemyDictionary> EnemyDictionary;

	UFUNCTION(BlueprintCallable)
	TArray<FWaveSpawnConfig> GetWaves() const;
	
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxSpawnRadius = 1400.f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MinSpawnRadius = 1100.f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 MaxNormalEnemyNum = 400;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxEliteEnemyNum = 5;
	
};

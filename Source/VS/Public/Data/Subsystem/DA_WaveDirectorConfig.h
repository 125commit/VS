// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DA_WaveDirectorConfig.generated.h"

USTRUCT(BlueprintType)
struct FWaveSpawnConfig
{
    GENERATED_BODY()
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TSubclassOf<AActor> EnemyClass;
    
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<FWaveSpawnConfig> Waves;
	
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxSpawnRadius = 1400.f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MinSpawnRadius = 1100.f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    int32 MaxNormalEnemyNum = 400;
	
};

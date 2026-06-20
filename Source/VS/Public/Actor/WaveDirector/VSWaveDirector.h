// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Data/Subsystem/DA_WaveDirectorConfig.h"
#include "VSWaveDirector.generated.h"

class AActor;
class UVSEnemyManager;

UCLASS()
class VS_API AVSWaveDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	AVSWaveDirector();
	virtual void Tick(float DeltaTime) override;
	
	//TODO: Set BP
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WaveDirector")
	TObjectPtr<UDA_WaveDirectorConfig> WaveConfig;
	
protected:
	virtual void BeginPlay() override;


private:
	void ProcessSpawn();
	int32 TrySpawnEnemiesWave(const FWaveSpawnConfig& Wave, UVSEnemyManager* EnemyManager,
				   int32 CurrentCount, int32 MaxCount, bool bSpawnAsElite);
	
	FVector GetEnemyRandomSpawnLocation();
	
	// 每条 Wave 各自的上次刷怪时间，互不干扰
	TArray<float> WaveLastSpawnTimes;
	
	float ElapsedTime = 0.f;
	

};

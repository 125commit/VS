// LAvid

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VSWaveDirector.generated.h"

USTRUCT(BlueprintType)
struct FWaveDirectorStruct
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = "WaveDirector")
	TSubclassOf<AActor> EnemyClass;
	
	UPROPERTY(EditAnywhere, Category = "WaveDirector")
	float StartTime = 0.f;
	
	UPROPERTY(EditAnywhere, Category = "WaveDirector")
	float EndTime = 300.f;
	
	UPROPERTY(EditAnywhere, Category = "WaveDirector")
	float SpawnInterval = 8.f;
	
	UPROPERTY(EditAnywhere, Category = "WaveDirector")
	int32 EnemyNumPreWave = 10;
	
	UPROPERTY(EditAnywhere, Category = "WaveDirector")
	bool bIsElite = false;
	
	float LastSpawnTime = 0.f;
};


UCLASS()
class VS_API AVSWaveDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	AVSWaveDirector();
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveDirector")
	TArray<FWaveDirectorStruct> WaveDirectorStruct;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveDirector")
	float MaxSpawnRadius = 1400.f; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveDirector")
	float MinSpawnRadius = 1100.f; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WaveDirector")
	int32 MaxNormalEnemyNum = 400;
	
protected:
	virtual void BeginPlay() override;


private:
	void ProcessSpawn();
	void Recycled();
	
	//生成精英怪
	void SpawnEliteEnemies();
	FTimerHandle TimerHandle;
	
	FVector GetEnemyRandomSpawnLocation();
	
	float ElapsedTime = 0.f;
	

};

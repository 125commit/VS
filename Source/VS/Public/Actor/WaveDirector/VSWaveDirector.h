// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Data/Subsystem/DA_WaveDirectorConfig.h"
#include "VSWaveDirector.generated.h"

class AActor;

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
	
	FVector GetEnemyRandomSpawnLocation();
	
	float LastSpawnTime = 0.f;
	float ElapsedTime = 0.f;
	

};

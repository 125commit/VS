// LAvid


#include "Data/Subsystem/DA_WaveDirectorConfig.h"

TArray<FWaveSpawnConfig> UDA_WaveDirectorConfig::GetWaves() const
{
	TArray<FWaveSpawnConfig> ResultArray;
	if (!WavesTable) return ResultArray;

	TArray<FWaveSpawnConfig*> AllRows;
	WavesTable->GetAllRows<FWaveSpawnConfig>(TEXT("GetWaves"), AllRows);

	for (FWaveSpawnConfig* Row : AllRows)
	{
		if (Row)
		{
			ResultArray.Add(*Row);
		}
	}
	return ResultArray;
}

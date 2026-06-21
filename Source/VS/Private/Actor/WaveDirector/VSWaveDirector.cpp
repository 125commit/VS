// LAvid


#include "Actor/WaveDirector/VSWaveDirector.h"
#include "GameFramework/Actor.h"
#include "Character/VSEnemy.h"
#include "Subsystem/VSEnemyManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

AVSWaveDirector::AVSWaveDirector()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVSWaveDirector::BeginPlay()
{
	Super::BeginPlay();
	ElapsedTime = 0.f;
	
	//初始化 WaveLastSpawnTimes 数组，先占位 (预分配)
	if (WaveConfig)
	{
		WaveLastSpawnTimes.Init(0.f, WaveConfig->GetWaves().Num());
	}
}

void AVSWaveDirector::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ElapsedTime += DeltaTime;
	ProcessSpawn();
}

void AVSWaveDirector::ProcessSpawn()
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	UVSEnemyManager* EnemyManager = World->GetSubsystem<UVSEnemyManager>();
	if (!EnemyManager) return;
	
	int32 CurrentNormalCount = EnemyManager->GetActiveNormalEnemiesCount();
	int32 CurrentEliteCount  = EnemyManager->GetActiveEliteEnemiesCount();
	
	
	TArray<FWaveSpawnConfig> AllWaves = WaveConfig->GetWaves();
	for (int32 WaveIndex = 0; WaveIndex < AllWaves.Num(); ++WaveIndex)
	{
		const FWaveSpawnConfig& Wave = AllWaves[WaveIndex];
		
		//如果数组长度还是不够，就用 AddZeroed 进行动态扩容
		if (WaveIndex >= WaveLastSpawnTimes.Num())
		{
			WaveLastSpawnTimes.AddZeroed(WaveIndex - WaveLastSpawnTimes.Num() + 1);
		}
		
		// 不在时间窗口内，跳过
		if (ElapsedTime <= Wave.StartTime || ElapsedTime >= Wave.EndTime)
		{
			continue;
		}
		
		// 该 Wave 自己的间隔未到，跳过
		if (ElapsedTime - WaveLastSpawnTimes[WaveIndex] <= Wave.SpawnInterval)
		{
			continue;
		}
		
		int32 SpawnedCount = 0;
		if (Wave.bIsElite)  //精英怪
		{
			SpawnedCount = TrySpawnEnemiesWave(
				Wave, EnemyManager,
				CurrentEliteCount, WaveConfig->MaxEliteEnemyNum,
				true
			);
			CurrentEliteCount += SpawnedCount;
		}
		else   //普通怪
		{
			SpawnedCount = TrySpawnEnemiesWave(
				Wave, EnemyManager,
				CurrentNormalCount, WaveConfig->MaxNormalEnemyNum,
				false
			);
			CurrentNormalCount += SpawnedCount;
		}
		
		//Spawn成功，把这一波的 WaveLastSpawnTimes 设为当前时间 ElapsedTime
		if (SpawnedCount > 0)
		{
			WaveLastSpawnTimes[WaveIndex] = ElapsedTime;
		}
	}
	
}

int32 AVSWaveDirector::TrySpawnEnemiesWave(const FWaveSpawnConfig& Wave, UVSEnemyManager* EnemyManager, int32 CurrentCount,
	int32 MaxCount, bool bSpawnAsElite)
{
	int32 AvailableCount = FMath::Max(0, MaxCount - CurrentCount);
	if (AvailableCount <= 0 ) return 0;
	
	const int32 NeedToSpawn = FMath::Min(FMath::Max(Wave.EnemyNumPerWave, 1),AvailableCount);
	
	TSubclassOf<AVSEnemy> EnemyClassToSpawn = nullptr;
	if (WaveConfig && WaveConfig->EnemyDictionary)
	{
		EnemyClassToSpawn = WaveConfig->EnemyDictionary->GetEnemyClass(Wave.EnemyType);
	}
	
	if (!EnemyClassToSpawn) return 0;
	
	int32 SpawnCount = 0;
	for (int32 i = 0; i < NeedToSpawn; ++ i)
	{
		const FVector SpawnLocation = GetEnemyRandomSpawnLocation();
		if (AActor* Actor = EnemyManager->SpawnEnemiesFromPool(EnemyClassToSpawn, SpawnLocation))
		{
			if (AVSEnemy* Enemy = Cast<AVSEnemy>(Actor))
			{
				if (bSpawnAsElite)
				{
					Enemy->SetIsElite(true);
				}
			}
			++ SpawnCount;
		}
	}
	return SpawnCount;
}


FVector AVSWaveDirector::GetEnemyRandomSpawnLocation()
{
	FVector PlayerPos = FVector::ZeroVector;
	if (const ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		PlayerPos = Player->GetActorLocation();
	}
	
	//生成一个360°的圆弧
	const float RandomYaw = FMath::FRandRange(0.0f, 360.0f);
	const FVector Direction = FVector(FMath::Cos(FMath::DegreesToRadians(RandomYaw)), FMath::Sin(FMath::DegreesToRadians(RandomYaw)), 0.0f);
	
	const float SpawnDistance = FMath::FRandRange(WaveConfig->MinSpawnRadius, WaveConfig->MaxSpawnRadius);
	
	FVector SpawnLocation = PlayerPos + (Direction * SpawnDistance);
	SpawnLocation.Z = PlayerPos.Z;
	
	return SpawnLocation;
}




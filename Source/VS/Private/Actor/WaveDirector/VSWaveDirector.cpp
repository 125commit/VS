// LAvid


#include "Actor/WaveDirector/VSWaveDirector.h"
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
	
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AVSWaveDirector::SpawnEliteEnemies, 60.f, true);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AVSWaveDirector::Recycled, 5.f, true);
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
	
	int32 CurrentNormalEnemiesNum = EnemyManager->GetActiveNormalEnemiesCount();
	
	for (FWaveDirectorStruct& Wave : WaveDirectorStruct)
	{
		if (ElapsedTime > Wave.StartTime && ElapsedTime < Wave.EndTime)
		{
			if (ElapsedTime - Wave.LastSpawnTime > Wave.SpawnInterval)
			{
				if (CurrentNormalEnemiesNum < MaxNormalEnemyNum)
				{
					if (Wave.bIsElite) return;
					
					int32 NeedToSpawn = FMath::Min(Wave.EnemyNumPreWave, MaxNormalEnemyNum - CurrentNormalEnemiesNum);
					
					for (int32 i = 0; i < NeedToSpawn; i ++)
					{
						const FVector SpawnLocation = GetEnemyRandomSpawnLocation();
						EnemyManager->SpawnEnemiesFromPool(Wave.EnemyClass, SpawnLocation);
						CurrentNormalEnemiesNum += 1;
					}
					Wave.LastSpawnTime = ElapsedTime;
				}
			}
		}
	}
	
}

//测试：每隔一定时间回收一些
void AVSWaveDirector::Recycled()
{
	UWorld* World = GetWorld();
	if (!World) return;
	
	UVSEnemyManager* EnemyManager = World->GetSubsystem<UVSEnemyManager>();
	if (!EnemyManager) return;
	
	int32 CurrentNormalEnemiesNum = EnemyManager->GetActiveNormalEnemiesCount();
	
	for (int32 i = 0; i < 5; i ++)
	{
		AActor* Enemy = EnemyManager->ActiveEnemies[i];
		EnemyManager->ReturnEnemiesToPool(Enemy);
		CurrentNormalEnemiesNum -= 1;
	}
}


void AVSWaveDirector::SpawnEliteEnemies()
{
	//TODO：所有数据自己获取
	//EnemyManager->SpawnEnemiesFromPool(Wave.EnemyClass, SpawnLocation);
	//Wave.LastSpawnTime = ElapsedTime;
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
	
	const float SpawnDistance = FMath::FRandRange(MinSpawnRadius, MaxSpawnRadius);
	
	FVector SpawnLocation = PlayerPos + (Direction * SpawnDistance);
	SpawnLocation.Z = PlayerPos.Z;
	
	return SpawnLocation;
}




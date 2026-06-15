// LAvid


#include "Actor/WaveDirector/VSWaveDirector.h"

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
	
	int32 CurrentEnemiesNum = EnemyManager->GetActiveNormalEnemiesCount();
	
	for (FWaveDirectorStruct& Wave : WaveDirectorStruct)
	{
		if (ElapsedTime > Wave.StartTime && ElapsedTime < Wave.EndTime)
		{
			if (ElapsedTime - Wave.LastSpawnTime > Wave.SpawnInterval)
			{
				//Spawn Elite
				if (Wave.bIsElite)
				{
					const FVector SpawnLocation = GetEnemyRandomSpawnLocation();
					if (AActor* Actor = EnemyManager->SpawnEnemiesFromPool(Wave.EnemyClass, SpawnLocation))
					{
						if (AVSEnemy* Enemy = Cast<AVSEnemy>(Actor))
						{
							Enemy->SetIsElite(true);
						}
						CurrentEnemiesNum += 1;
						Wave.LastSpawnTime = ElapsedTime;
					}
				}
				//Spawn normal enemy
				else
				{
					if (CurrentEnemiesNum < MaxNormalEnemyNum)
					{
						int32 NeedToSpawn = FMath::Min(Wave.EnemyNumPreWave, MaxNormalEnemyNum - CurrentEnemiesNum);
					
						for (int32 i = 0; i < NeedToSpawn; i ++)
						{
							const FVector SpawnLocation = GetEnemyRandomSpawnLocation();
							EnemyManager->SpawnEnemiesFromPool(Wave.EnemyClass, SpawnLocation);
							CurrentEnemiesNum += 1;
						}
						Wave.LastSpawnTime = ElapsedTime;
					}
					
				}
				
			}
		}
	}
	
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




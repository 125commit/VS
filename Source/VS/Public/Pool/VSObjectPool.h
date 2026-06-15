// LAvid

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VSObjectPool.generated.h"

USTRUCT()
struct FCustomActorPool
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<AActor*> UnusedActors;
};

/**
 * 
 */
UCLASS()
class VS_API UVSObjectPool : public UObject
{
	GENERATED_BODY()
	
public:
	//大退的时候清空对象池
	void ClearPool();
	
	AActor* GetActorFromPool(UWorld* World, const TSubclassOf<AActor> ActorClass, const FVector& Location, const FRotator& Rotation);
	void ReturnActorToPool(AActor* Actor);
	
private:
	TMap<TSubclassOf<AActor>, FCustomActorPool> PoolMap;
};

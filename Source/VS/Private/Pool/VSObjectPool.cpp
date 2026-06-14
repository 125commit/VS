// LAvid


#include "Pool/VSObjectPool.h"

void UVSObjectPool::ClearPool()
{
	PoolMap.Empty();
}

AActor* UVSObjectPool::GetActorFromPool(UWorld* World, const TSubclassOf<AActor> ActorClass, const FVector& Location,
                                        const FRotator& Rotation)
{
	if (!World || !ActorClass) return nullptr;
	
	if (PoolMap.Contains(ActorClass))
	{
		FCustomActorPool& TargetPool = PoolMap[ActorClass];
		while (TargetPool.UnusedActors.Num() > 0)
		{
			AActor* ActionActor = TargetPool.UnusedActors.Pop(false);
			if (IsValid(ActionActor))
			{
				ActionActor->SetActorLocationAndRotation(Location, Rotation);
				ActionActor->SetActorHiddenInGame(false);
				ActionActor->SetActorEnableCollision(true);
				return ActionActor;
			}
		}
	}
	
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	return World->SpawnActor<AActor>(ActorClass, Location, Rotation, ActorSpawnParameters);
	
}

void UVSObjectPool::ReturnActorToPool(AActor* Actor)
{
	if (!IsValid(Actor)) return;
	
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorEnableCollision(false);
	
	if (USceneComponent* RootComponent =  Actor->GetRootComponent())
	{
		if (UPrimitiveComponent* PhysicalComponent = Cast<UPrimitiveComponent>(RootComponent))
		{
			PhysicalComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
			PhysicalComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		}
	}
	
	const TSubclassOf<AActor> ActorClass = Actor->GetClass();
	FCustomActorPool& ActorPool = PoolMap.FindOrAdd(ActorClass);
	ActorPool.UnusedActors.Add(Actor);
	
	
}

// LAvid

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "VSWeaponSubsysem.generated.h"

class AVS_WeaponActor;
class UVSObjectPool;
struct FVSWeaponInitParams;
/**
 * 
 */
UCLASS()
class VS_API UVSWeaponSubsysem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	AVS_WeaponActor* SpawnWeaponFromPool(
		TSubclassOf<AVS_WeaponActor> WeaponActorClass,
		const FTransform& SpawnTransform,
		AActor* AvatarActor,
		const FVSWeaponInitParams& InitParams);
	
	void ReturnWeaponToPool(AVS_WeaponActor* Weapon);
	
private:
	UPROPERTY()
	TObjectPtr<UVSObjectPool> WeaponPool = nullptr;
	
	UPROPERTY()
	TArray<TObjectPtr<AVS_WeaponActor>> ActiveWeapons;
};

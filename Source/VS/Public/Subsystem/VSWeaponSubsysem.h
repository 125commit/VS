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
	
	/** 查找某 Owner 身上已激活的指定类型武器（ Garlic 常驻武器复用） */
	AVS_WeaponActor* FindActiveWeapon(AActor* Owner, TSubclassOf<AVS_WeaponActor> WeaponActorClass) const;

	
private:
	UPROPERTY()
	TObjectPtr<UVSObjectPool> WeaponPool = nullptr;
	
	UPROPERTY()
	TArray<TObjectPtr<AVS_WeaponActor>> ActiveWeapons;
};

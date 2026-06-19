// LAvid


#include "Subsystem/VSWeaponSubsysem.h"
#include "Actor/VS_WeaponActor.h"
#include "Pool/VSObjectPool.h"


void UVSWeaponSubsysem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	WeaponPool = NewObject<UVSObjectPool>(this);
}

void UVSWeaponSubsysem::Deinitialize()
{
	TArray<TObjectPtr<AVS_WeaponActor>> WeaponsToReturn = MoveTemp(ActiveWeapons);
	for (AVS_WeaponActor* Weapon : WeaponsToReturn)
	{
		if (!Weapon || !WeaponPool) continue;
		
		Weapon->DeactivateWeapon();
		WeaponPool->ReturnActorToPool(Weapon);
	}
	ActiveWeapons.Empty();
	
	if (WeaponPool)
	{
		WeaponPool->ClearPool();
	}
	
	Super::Deinitialize();
}

AVS_WeaponActor* UVSWeaponSubsysem::SpawnWeaponFromPool(TSubclassOf<AVS_WeaponActor> WeaponActorClass,
	const FTransform& SpawnTransform, AActor* AvatarActor, const FVSWeaponInitParams& InitParams)
{
	UWorld* World = GetWorld(); 
	if (!World) return nullptr;
	
	if (!WeaponActorClass || !AvatarActor || !WeaponPool) return nullptr;
	
	AVS_WeaponActor* Weapon = Cast<AVS_WeaponActor>( WeaponPool->GetActorFromPool(World, WeaponActorClass, 
		SpawnTransform.GetLocation(), SpawnTransform.Rotator()));
	
	if (!Weapon) return nullptr;

	Weapon->ActivateWeapon(InitParams, AvatarActor, Cast<APawn>(AvatarActor));
	ActiveWeapons.Add(Weapon);
	
	return Weapon;
}

void UVSWeaponSubsysem::ReturnWeaponToPool(AVS_WeaponActor* Weapon)
{
	if (!Weapon || !WeaponPool) return;
	
	Weapon->DeactivateWeapon();
	ActiveWeapons.RemoveSingleSwap(Weapon);
	WeaponPool->ReturnActorToPool(Weapon);
}

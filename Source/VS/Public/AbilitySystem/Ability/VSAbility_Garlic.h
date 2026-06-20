// LAvid

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/VS_WeaponAbility.h"
#include "VSAbility_Garlic.generated.h"

class AVSGarlicActor;
/**
 * 
 */
UCLASS()
class VS_API UVSAbility_Garlic : public UVS_WeaponAbility
{
	GENERATED_BODY()
	
public:
	UVSAbility_Garlic();
	
protected:
	virtual void ExecuteFire(const FVSAbilityRuntimeStats& Stats) override;
	
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
	
	// Garlic不会回池，所以要本地缓存，表示玩家已经有 GarlicActor 了
	UPROPERTY()
	TWeakObjectPtr<AVSGarlicActor> ActiveGarlicActor;
	
};

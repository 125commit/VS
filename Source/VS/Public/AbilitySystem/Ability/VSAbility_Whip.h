// LAvid

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/VS_WeaponAbility.h"
#include "VSAbility_Whip.generated.h"

class AVSWhipActor;
/**
 * 
 */
UCLASS()
class VS_API UVSAbility_Whip : public UVS_WeaponAbility
{
	GENERATED_BODY()
	
public:
	UVSAbility_Whip();
	
protected:
	virtual void ExecuteFire(const FVSAbilityRuntimeStats& Stats) override;


};

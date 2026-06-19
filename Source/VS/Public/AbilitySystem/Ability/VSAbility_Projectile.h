// LAvid

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/VS_WeaponAbility.h"
#include "VSAbility_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class VS_API UVSAbility_Projectile : public UVS_WeaponAbility
{
	GENERATED_BODY()
	
public: 
	UVSAbility_Projectile();

protected:
	virtual void ExecuteFire(const FVSAbilityRuntimeStats& Stats) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "VS|Projectile")
	float SpawnForwardOffset = 100.f;

	// 射程，只有此范围内的敌人才会触发发射
	UPROPERTY(EditDefaultsOnly, Category = "VS|Projectile")
	float FireRange = 1500.f;
};

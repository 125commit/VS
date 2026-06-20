// LAvid

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/VS_WeaponAbility.h"
#include "Data/VSProjectileType.h"
#include "VSAbility_Projectile.generated.h"

class AVSEnemy;
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
	virtual bool TryGetWeaponFireContext(FVSWeaponFireContext& OutContext) const override;
	virtual void ExecuteFire(const FVSAbilityRuntimeStats& Stats) override;
	
	//攻击和飞行模式：
	UPROPERTY(EditDefaultsOnly, Category = "VS|Projectile")
	EVSProjectileTargetMode TargetMode = EVSProjectileTargetMode::NearestEnemy;
	
	UPROPERTY(EditDefaultsOnly, Category = "VS|Projectile")
	EVSProjectileMovementMode MovementMode = EVSProjectileMovementMode::Homing;
	
	UPROPERTY(EditDefaultsOnly, Category = "VS|Projectile")
	float SpawnForwardOffset = 100.f;

	// 射程，只有此范围内的敌人才会触发发射
	UPROPERTY(EditDefaultsOnly, Category = "VS|Projectile")
	float FireRange = 800.f;
	

	
	
	
	
};

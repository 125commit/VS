// LAvid


#include "AbilitySystem/Ability/EvolvedAbility/VSAbility_EvolvedFireWand.h"

#include "VSGameplayTags.h"

UVSAbility_EvolvedFireWand::UVSAbility_EvolvedFireWand()
{
	SetupWeaponAbility(FVSGameplayTags::Get().Abilities_Weapon_EvolvedFireWand);
	
	TargetMode   = EVSProjectileTargetMode::RandomEnemy;
	MovementMode = EVSProjectileMovementMode::Straight;
}

// LAvid


#include "AbilitySystem/Ability/EvolvedAbility/VSAbility_EvolvedMagicWand.h"

#include "VSGameplayTags.h"

UVSAbility_EvolvedMagicWand::UVSAbility_EvolvedMagicWand()
{
	SetupWeaponAbility(FVSGameplayTags::Get().Abilities_Weapon_EvolvedMagicWand);
	
	TargetMode   = EVSProjectileTargetMode::NearestEnemy;
	MovementMode = EVSProjectileMovementMode::Homing;
	
}

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "VS_GameplayAbility.generated.h"

UCLASS()
class VS_API UVS_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	// ==========================================
	// Tag
	// ==========================================

	// 1. 本技能的唯一身份 Tag (例如配置为：Abilities.Weapon.Garlic)
	// 策划配蓝图子类时必填！
	UPROPERTY(EditDefaultsOnly, Category="VS|Tags")
	FGameplayTag AbilityTag;

	// 2. 本技能的类型 Tag (例如配置为：Abilities.Type.Weapon)
	// 用于告知 ASC 这是武器还是被动，用来卡槽位上限！
	UPROPERTY(EditDefaultsOnly, Category="VS|Tags")
	FGameplayTag AbilityTypeTag;

};
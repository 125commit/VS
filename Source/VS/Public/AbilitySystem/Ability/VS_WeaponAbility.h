// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/VS_GameplayAbility.h"
#include "VS_WeaponAbility.generated.h"

class AVS_WeaponActor;

/**
 * 武器技能基类 (大脑)
 * 负责大循环倒计时，并生成包含碰撞和特效的实体打手
 */
UCLASS()
class VS_API UVS_WeaponAbility : public UVS_GameplayAbility
{
	GENERATED_BODY()
	
public:
	// 技能激活的主入口
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	// -------------------------------------------------------------

	// 设计师在蓝图里配置：要生成的武器实体类 (如：大蒜Actor、神鞭Actor)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VS|Weapon")
	TSubclassOf<AVS_WeaponActor> WeaponActorClass;

	// 设计师在蓝图里配置：全游戏技能大字典 (用于查询本技能各级别对应的基础伤害)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VS|Weapon")
	TObjectPtr<class UDA_AbilityInfo> GlobalAbilityInfoData;

	// -------------------------------------------------------------
	// 循环与结算逻辑
	// -------------------------------------------------------------

	// 循环触发的核心函数
	UFUNCTION()
	void OnWeaponFire();

	// 获取角色属性的辅助函数
	float GetAttributeValue(const FGameplayAttribute& Attribute) const;
};

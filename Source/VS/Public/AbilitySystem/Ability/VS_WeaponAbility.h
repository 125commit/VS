// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/VS_GameplayAbility.h"
#include "VS_WeaponAbility.generated.h"

class AVS_WeaponActor;
struct FVSAbilityRuntimeStats;
struct FVSWeaponInitParams;
class UVSWeaponSubsysem;

struct FVSWeaponFireContext
{
	UWorld* World = nullptr;
	UVSWeaponSubsysem* WeaponSubsystem = nullptr;
	AActor* AvatarActor = nullptr;
	bool bIsValid = false;
};

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
	TObjectPtr<class UVSAbilityInfoData> GlobalAbilityInfoData;

	// ── 击退配置（每把武器在 GA 蓝图配置）──
		// 击退系数：每 1 点最终伤害产生多少 cm/s 击退初速度。0 = 该武器不击退
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VS|Knockback")
	float KnockbackPerDamage = 0.f;
	
		// 固定底力：保证低伤害也有最低击退（可为 0）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VS|Knockback")
	float KnockbackBaseForce = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VS|Knockback")
	float KnockbackDuration = 0.15f;
	
		// 同一武器再次命中同一敌人的最小间隔(秒)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VS|Knockback")
	float HitInterval = 0.2f;
	
	// -------------------------------------------------------------
	// 循环与结算逻辑
	// -------------------------------------------------------------

	// 循环触发的核心函数
	UFUNCTION()
	virtual void OnWeaponFire();
	
	/**
	 * 职责：返回两轮开火之间的等待间隔
	 * 原因：不同武器的"开火节奏"语义不同（魔杖=发射间隔、大蒜=周期伤害间隔、圣经=在场+空窗）
	 *       把这个会变的策略抽成虚函数，循环逻辑留在父类共享，子类只需重写本函数即可定制节奏，
	 *       避免把某个武器的特例硬塞进父类而污染其它武器
	 */
	virtual float GetNextFireDelay(const FVSAbilityRuntimeStats& Stats) const;
	
	virtual void ExecuteFire(const FVSAbilityRuntimeStats& Stats);
	
	/** BaseDamage(表) × Might(AttributeSet) */
	virtual float ComputeFinalDamage(float BaseDamage) const;
	
	// 获取角色属性的辅助函数
	float GetAttributeValue(const FGameplayAttribute& Attribute) const;
	
	//设置实例化策略、AbilityTypeTag 和 AbilityTag
	void SetupWeaponAbility(const FGameplayTag& InAbilityTag);
	
	//统一检查World、WeaponSubsystem、AvatarActor
	virtual bool TryGetWeaponFireContext(FVSWeaponFireContext& OutContext) const;
	
	//设置技能都会用到的数据的接口函数（Area、Damage都会用到；ProjectileSpeed部分会用到，就额外设置）
	FVSWeaponInitParams MakeBaseWeaponParams(const FVSAbilityRuntimeStats& Stats) const;
	
};

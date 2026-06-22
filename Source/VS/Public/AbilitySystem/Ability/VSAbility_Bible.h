// LAvid

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/VS_WeaponAbility.h"
#include "VSAbility_Bible.generated.h"

/**
 * 职责：国王圣经（魔法书）大脑——每轮按弹道数量沿圆周均分相位、生成多本书
 * 原因：圣经需要"在场 Duration → 空窗 Cooldown → 再出现"的闪烁节奏，
 *       且同时在场数量须恒等于 ProjectileCount，故重写开火间隔策略
 */
UCLASS()
class VS_API UVSAbility_Bible : public UVS_WeaponAbility
{
	GENERATED_BODY()

public:
	UVSAbility_Bible();

protected:
	// 单轮开火：生成本轮所有书
	virtual void ExecuteFire(const FVSAbilityRuntimeStats& Stats) override;

	// 重写开火间隔：下一轮 = 在场 Duration + 空窗 Cooldown，确保上一轮的书先退场再生成，避免叠加
	virtual float GetNextFireDelay(const FVSAbilityRuntimeStats& Stats) const override;
};

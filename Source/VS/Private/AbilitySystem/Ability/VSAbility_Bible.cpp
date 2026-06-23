// LAvid


#include "AbilitySystem/Ability/VSAbility_Bible.h"

#include "VSGameplayTags.h"
#include "Actor/VS_WeaponActor.h"
#include "Subsystem/VSWeaponSubsysem.h"
#include "Data/VSAbilityInfoData.h" // 需要FVSAbilityRuntimeStats 完整定义以访问其成员

UVSAbility_Bible::UVSAbility_Bible()
{
	// 绑定身份 Tag 为"魔法书"，并设实例化策略 / 类型 Tag（基类统一处理）
	SetupWeaponAbility(FVSGameplayTags::Get().Abilities_Weapon_MagicBook);
}

void UVSAbility_Bible::ExecuteFire(const FVSAbilityRuntimeStats& Stats)
{
	// 统一校验 World / WeaponSubsystem / AvatarActor / WeaponActorClass
	FVSWeaponFireContext Context;
	if (!TryGetWeaponFireContext(Context)) return;

	// 弹道数量 = 本级书的数量，至少 1
	const int32 Count = FMath::Max(1, Stats.ProjectileCount);
	const FVector PlayerLoc = Context.AvatarActor->GetActorLocation();

	for (int32 i = 0; i < Count; ++i)
	{
		// 由表值（伤害/范围/时长/速度倍率等）生成基础参数包
		FVSWeaponInitParams Params = MakeBaseWeaponParams(Stats);

		// STEP: 多本书沿整圆均分相位；借用 FacingRotation.Yaw 把初始角传给 Actor（免新增字段）
		const float AngleDeg = 360.f / Count * i;
		Params.FacingRotation = FRotator(0.f, AngleDeg, 0.f);

		// NOTE: 在玩家位置生成即可，Actor 会在 ActivateWeapon 内立刻自我归位到环绕点
		const FTransform SpawnTransform(Params.FacingRotation, PlayerLoc);

		Context.WeaponSubsystem->SpawnWeaponFromPool(
			WeaponActorClass,
			SpawnTransform,
			Context.AvatarActor,
			Params);
	}
}

float UVSAbility_Bible::GetNextFireDelay(const FVSAbilityRuntimeStats& Stats) const
{
	// NOTE: 下一轮延后到"在场 Duration + 空窗 Cooldown"之后
	//       这样上一轮的书必定已全部回池，空窗期场上 0 本，同时在场数恒等于 ProjectileCount，
	//       从结构上消除"按周期生成"与"按寿命回收"两个独立定时器导致的叠加问题
	return Stats.bNoCooldown ? 0.f : (Stats.Duration + Stats.Cooldown);
}

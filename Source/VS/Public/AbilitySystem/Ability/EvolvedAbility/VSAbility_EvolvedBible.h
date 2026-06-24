// LAvid

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/VSAbility_Bible.h"
#include "VSAbility_EvolvedBible.generated.h"

/**
 * 
 */
UCLASS()
class VS_API UVSAbility_EvolvedBible : public UVSAbility_Bible
{
	GENERATED_BODY()
	
public:
	UVSAbility_EvolvedBible();
	
protected:
	// 真常驻：激活时生成一次永久书，之后不进入开火循环
	virtual void OnWeaponFire() override;
	
	// 生成并记录永久书（供 EndAbility 回收）；幂等，避免重复生成
	virtual void ExecuteFire(const FVSAbilityRuntimeStats& Stats) override;
	
	// 技能结束：常驻书不会自己回池，必须手动回收，否则泄漏
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:
	// 已生成的常驻书（弱引用，回收时清理）
	TArray<TWeakObjectPtr<AVS_WeaponActor>> SpawnedBooks;
};


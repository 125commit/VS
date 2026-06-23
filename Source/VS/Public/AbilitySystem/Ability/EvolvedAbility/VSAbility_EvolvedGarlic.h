// LAvid

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/VSAbility_Garlic.h"
#include "VSAbility_EvolvedGarlic.generated.h"

class AVSEnemy;
/**
 * 
 */
UCLASS()
class VS_API UVSAbility_EvolvedGarlic : public UVSAbility_Garlic
{
	GENERATED_BODY()
	
public:
	UVSAbility_EvolvedGarlic();
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	// 注入成长伤害：最终伤害 = (Base × Might) + 当前增幅
	virtual float ComputeFinalDamage(float BaseDamage) const override;
	
	// 杀敌回血
	UPROPERTY(EditDefaultsOnly, Category = "VS|SoulEater")
	float HealPerKill = 1.f;
	
	// 每累计回血 xx 点 → 伤害 +1
	UPROPERTY(EditDefaultsOnly, Category = "VS|SoulEater")
	float HealPerDamageStep = 60.f;
	
	// 伤害增幅上限
	UPROPERTY(EditDefaultsOnly, Category = "VS|SoulEater")
	
	float MaxDamageBonus = 400.f;
private:
	
	void HandleEnemyKilled(AVSEnemy* Victim, AActor* Causer);
	
	// 运行时状态（武器技能是 InstancedPerActor，状态会持久保留）
	float AccumulatedHeal   = 0.f;
	float CurrentDamageBonus = 0.f;
	FDelegateHandle KilledHandle;
};

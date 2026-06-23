// LAvid

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/VSAbility_Whip.h"
#include "VSAbility_EvolvedWhip.generated.h"

class AVSEnemy;
/**
 * 
 */
UCLASS()
class VS_API UVSAbility_EvolvedWhip : public UVSAbility_Whip
{
	GENERATED_BODY()
	
public:
	UVSAbility_EvolvedWhip();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	// 暴击：吸血鞭攻击必定暴击，最终伤害 ×CritMultiplier
	virtual float ComputeFinalDamage(float BaseDamage) const override;
	
	// 暴击的伤害倍率
	UPROPERTY(EditDefaultsOnly, Category = "VS|BloodyTear")
	float CritMultiplier = 2.f;
	
	//攻击恢复
	UPROPERTY(EditDefaultsOnly, Category = "VS|BloodyTear")
	float HealPerHit = 8.f;
	
private:
	void HandleEnemyDamaged(AVSEnemy* Victim, float Damage, AActor* Causer);
	FDelegateHandle DamagedHandle;
};

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/VS_GameplayAbility.h"
#include "VS_PassiveAbility.generated.h"

class UVSAbilityInfoData;
struct FScalableFloat;
/**
 * 被动技能基类
 * 激活后常驻后台，监听升级事件并动态刷新自身的属性加成 GE
 */
UCLASS()
class VS_API UVS_PassiveAbility : public UVS_GameplayAbility
{
	GENERATED_BODY()

public:
	UVS_PassiveAbility();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UFUNCTION()
	void OnUpgradeEventReceived(FGameplayEventData Payload);

	void ApplyPassiveEffect();

	// 在蓝图里配置：全游戏技能字典 (用于查询本被动技能各级别对应的加成数值)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VS|Passive")
	TObjectPtr<UVSAbilityInfoData> GlobalAbilityInfoData;

	// 在蓝图里配置：增加属性的永久 GameplayEffect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VS|Passive")
	TSubclassOf<UGameplayEffect> PassiveEffect;

	// 在蓝图里配置：SetByCaller 传值用的标签 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VS|Passive")
	FGameplayTag SetByCallerTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VS|Passive")
	FScalableFloat PassiveMagnitude;

private:
	// 记录当前挂在身上的 GE 句柄，方便升级时移除并刷新
	FActiveGameplayEffectHandle ActiveEffectHandle;
};

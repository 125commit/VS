#include "AbilitySystem/Ability/VS_PassiveAbility.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Data/DA_AbilityInfo.h"

UVS_PassiveAbility::UVS_PassiveAbility()
{
	// 被动技能必须实例化 常驻内存，才能在后台一直监听后续的升级事件
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UVS_PassiveAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (HasAuthority(&CurrentActivationInfo))
	{
		// 1. 首次激活，查表并应用初始属性加成
		ApplyPassiveEffect();

		// 2. 挂载异步任务，监听全局的 "Ability.Upgrade" 事件
		FGameplayTag UpgradeEventTag = FGameplayTag::RequestGameplayTag(FName("Ability.Upgrade"));
		UAbilityTask_WaitGameplayEvent* WaitEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, UpgradeEventTag, nullptr, false, true);
		WaitEventTask->EventReceived.AddDynamic(this, &UVS_PassiveAbility::OnUpgradeEventReceived);
		WaitEventTask->ReadyForActivation();
	}
}

void UVS_PassiveAbility::OnUpgradeEventReceived(FGameplayEventData Payload)
{
	if (!HasAuthority(&CurrentActivationInfo)) return;

	// 只要收到了升级事件广播，说明有技能升了级。
	// 这里直接进行无脑刷新。因为读取 GetAbilityLevel() 是幂等的：
	// 没升级的被动，刷新后加成不变；升了级的被动，瞬间拉高加成数值！
	ApplyPassiveEffect();
}

void UVS_PassiveAbility::ApplyPassiveEffect()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !PassiveEffect) return;

	// 1. 如果身上已经有旧的被动加成，先扒掉
	if (ActiveEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
		ActiveEffectHandle.Invalidate();
	}

	// 2. 通过 CT 获取数值
	const float PassiveValue = PassiveMagnitude.GetValueAtLevel(GetAbilityLevel());

	// 3. 构建新的 GE，将数值通过 SetByCaller 塞进去
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(PassiveEffect, 1.0f, ContextHandle);
	
	if (SpecHandle.IsValid())
	{
		if (SetByCallerTag.IsValid())
		{
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(SetByCallerTag, PassiveValue);
		}
		
		// 4. 应用 GE 并保存句柄！
		ActiveEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

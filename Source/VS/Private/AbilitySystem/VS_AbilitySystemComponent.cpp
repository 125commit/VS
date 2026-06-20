#include "AbilitySystem/VS_AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Ability/VS_GameplayAbility.h"

void UVS_AbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UVS_AbilitySystemComponent::ClientEffectApplied);
}

TArray<FVSOwnedAbilityInfo> UVS_AbilitySystemComponent::GetOwnedAbilities() const
{
	TArray<FVSOwnedAbilityInfo> Result;
	// 遍历 ASC 内部当前所有被赋予的技能 (Spec)
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		// 尝试强转为我们特制的 VS 技能基类
		if (const UVS_GameplayAbility* VSAbility = Cast<UVS_GameplayAbility>(Spec.Ability))
		{
			FVSOwnedAbilityInfo Info;
			// 完美提取出策划在蓝图里配置的烙印！
			Info.AbilityTag = VSAbility->AbilityTag;
			Info.AbilityTypeTag = VSAbility->AbilityTypeTag;
			Info.Level = Spec.Level;
			
			Result.Add(Info);
		}
	}
	return Result;
}

// =========================================================================
// 吸血鬼幸存者：升级菜单选卡核心入口
// =========================================================================
void UVS_AbilitySystemComponent::ServerUpgradeAbility_Implementation(TSubclassOf<UGameplayAbility> WeaponAbilityClass)
{
	if (!WeaponAbilityClass) return;

	// 1. 查表：我的技能栏里是不是已经有这个技能了？
	FGameplayAbilitySpec* ExistingSpec = FindAbilitySpecFromClass(WeaponAbilityClass);
	
	if (ExistingSpec)
	{
		// 【情况 A：重复即升级】
		ExistingSpec->Level += 1;
		
		// 强制同步脏数据到客户端
		MarkAbilitySpecDirty(*ExistingSpec);
		
		// TODO:发送升级事件，让被动技能刷新属性
		FGameplayEventData Payload;
		Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Ability.Upgrade"));
		Payload.EventMagnitude = ExistingSpec->Level;
		HandleGameplayEvent(Payload.EventTag, &Payload);
	}
	else
	{
		// 【情况 B：获取即激活】
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(WeaponAbilityClass, 1);
		GiveAbility(AbilitySpec);
		
		// 吸血鬼核心：技能一旦拿到手，立刻无条件启动后台循环 (被动应用 GE，皮鞭启动 Timer)！
		TryActivateAbility(AbilitySpec.Handle);
	}
}

// =========================================================================
// 角色出生时自带的初始武器
// =========================================================================
void UVS_AbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;
	
	// 防止因为 Controller 重复附身（PossessedBy被调用多次）导致发两次初始武器！
	if (bStartupAbilitiesGiven) return;

	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		GiveAbility(AbilitySpec);
		
		// 初始武器同样必须立刻激活！
		TryActivateAbility(AbilitySpec.Handle);
	}

	bStartupAbilitiesGiven = true;
	AbilitiesGivenDelegate.Broadcast();
}

void UVS_AbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "VS_AbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);

UCLASS()
class VS_API UVS_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityActorInfoSet();

	// -----------------------------------------------------------------
	// Vampire Survivors 核心武器管线
	// -----------------------------------------------------------------

	// 赋予玩家初始武器/被动 (通常是角色天生自带的那个武器)
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	// 客户端在升级选卡菜单选中某个武器后，呼叫服务器
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerUpgradeWeapon(TSubclassOf<UGameplayAbility> WeaponAbilityClass);

	// -----------------------------------------------------------------
	// 委托与防火墙标志位
	// -----------------------------------------------------------------
	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;

	bool bStartupAbilitiesGiven = false;

protected:

	// 只有客户端会执行的 RPC：当被附加 GE 时，在客户端广播标签给 UI
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
};

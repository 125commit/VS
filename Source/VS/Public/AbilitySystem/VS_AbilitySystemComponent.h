// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "VS_AbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponEvolvedSignature, const FGameplayTag&, OldTag, const FGameplayTag&, NewTag);

USTRUCT(BlueprintType)
struct FVSOwnedAbilityInfo
{
	GENERATED_BODY()
	FGameplayTag AbilityTag;
	FGameplayTag AbilityTypeTag;
	int32 Level;
};


UCLASS()
class VS_API UVS_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityActorInfoSet();

	// -----------------------------------------------------------------
	// Vampire Survivors 核心武器管线
	// -----------------------------------------------------------------

	// 提取自身持有的所有有效技能的身份信息和等级
	TArray<FVSOwnedAbilityInfo> GetOwnedAbilities() const;
	
	// 赋予玩家初始武器/被动 (通常是角色天生自带的那个武器)
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	// 客户端在升级选卡菜单选中某个技能后，呼叫服务器
	UFUNCTION(Server, Reliable, BlueprintCallable)
	void ServerUpgradeAbility(TSubclassOf<UGameplayAbility> WeaponAbilityClass);

	// 记录进化时被消耗的武器Tag，用于从随机池里踢掉
	UPROPERTY(BlueprintReadOnly, Category = "Evolution")
	TArray<FGameplayTag> EvolvedHistoryWeapons;

	UFUNCTION(BlueprintCallable, Category = "Evolution")
	void AddHistoryWeapon(const FGameplayTag& WeaponTag);

	// 专门通知客户端：你的武器进化了，立刻把UI盖上去！
	UFUNCTION(Client, Reliable)
	void Client_NotifyWeaponEvolved(const FGameplayTag& OldTag, const FGameplayTag& NewTag);

	// -----------------------------------------------------------------
	// 委托与防火墙标志位
	// -----------------------------------------------------------------
	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Evolution")
	FWeaponEvolvedSignature OnWeaponEvolved;

	bool bStartupAbilitiesGiven = false;

protected:

	// 只有客户端会执行的 RPC：当被附加 GE 时，在客户端广播标签给 UI
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
};

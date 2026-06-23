#include "AbilitySystem/VS_AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Ability/VS_GameplayAbility.h"
#include "Player/VS_PlayerController.h"
#include "Data/VSAbilityInfoData.h"

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

	// 用于记录刚升完级/获取的技能 Tag 和新等级
	FGameplayTag UpdatedAbilityTag;
	int32 NewLevel = 1;

	// 1. 查表：我的技能栏里是不是已经有这个技能了？
	FGameplayAbilitySpec* ExistingSpec = FindAbilitySpecFromClass(WeaponAbilityClass);
	
	if (ExistingSpec)
	{
		// 【情况 A：重复即升级】
		ExistingSpec->Level += 1;
		NewLevel = ExistingSpec->Level;
		
		if (const UVS_GameplayAbility* VSAbility = Cast<UVS_GameplayAbility>(ExistingSpec->Ability))
		{
			UpdatedAbilityTag = VSAbility->AbilityTag;
		}

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
		FGameplayAbilitySpecHandle Handle = GiveAbility(AbilitySpec);
		NewLevel = 1;

		if (const UVS_GameplayAbility* VSAbility = Cast<UVS_GameplayAbility>(WeaponAbilityClass->GetDefaultObject()))
		{
			UpdatedAbilityTag = VSAbility->AbilityTag;
		}
		
		// 吸血鬼核心：技能一旦拿到手，立刻无条件启动后台循环 (被动应用 GE，皮鞭启动 Timer)！
		TryActivateAbility(Handle);
	}

	// -------------------------------------------------------------
	// 进化检测拦截：技能发放完后，检查是否触发了化学反应！
	// -------------------------------------------------------------
	if (AVS_PlayerController* VSPC = Cast<AVS_PlayerController>(GetOwner()))
	{
		if (VSPC->AbilityInfoData)
		{
			// 问一下全局大表，这个刚拿到手的技能，加上现有的等级，会不会触发进化？
			FGameplayTag EvolvedTag = VSPC->AbilityInfoData->CheckIfCausesEvolution(UpdatedAbilityTag, NewLevel, this);
			
			if (EvolvedTag.IsValid())
			{
				// 进化触发了！我们需要知道这套配方到底需要献祭哪个武器
				FGameplayTag WeaponToSacrificeTag;
				
				// 再次反查配方表，找出那个祭品武器的 Tag
				TArray<FVSEvolvedWeaponStatsRow*> AllEvolutionRows;
				VSPC->AbilityInfoData->GlobalEvolvedWeaponStatsTable->GetAllRows<FVSEvolvedWeaponStatsRow>(TEXT("ExecuteEvolution"), AllEvolutionRows);
				
				for (const FVSEvolvedWeaponStatsRow* Row : AllEvolutionRows)
				{
					if (!Row) continue;
					const TMap<FName, uint8*>& RowMap = VSPC->AbilityInfoData->GlobalEvolvedWeaponStatsTable->GetRowMap();
					for (auto& Pair : RowMap)
					{
						if (Pair.Value == reinterpret_cast<uint8*>(const_cast<FVSEvolvedWeaponStatsRow*>(Row)))
						{
							if (Pair.Key == EvolvedTag.GetTagName())
							{
								WeaponToSacrificeTag = Row->RequiredWeaponTag;
								break;
							}
						}
					}
					if (WeaponToSacrificeTag.IsValid()) break;
				}

				if (WeaponToSacrificeTag.IsValid())
				{
					// 1. 抹除旧武器 (ClearAbility)
					for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
					{
						if (const UVS_GameplayAbility* VSAbility = Cast<UVS_GameplayAbility>(Spec.Ability))
						{
							if (VSAbility->AbilityTag.MatchesTagExact(WeaponToSacrificeTag))
							{
								ClearAbility(Spec.Handle);
								break;
							}
						}
					}

					// 2. 将旧武器塞进历史数组（防止卡池再刷出来）
					AddHistoryWeapon(WeaponToSacrificeTag);

					// 3. 发放新进化武器！
					FVSAbilityInfo EvolvedInfo = VSPC->AbilityInfoData->FindAbilityInfoForTag(EvolvedTag);
					if (EvolvedInfo.AbilityClass)
					{
						FGameplayAbilitySpec EvolvedSpec = FGameplayAbilitySpec(EvolvedInfo.AbilityClass, 1);
						FGameplayAbilitySpecHandle EvolvedHandle = GiveAbility(EvolvedSpec);
						TryActivateAbility(EvolvedHandle);

						// 4. 通知客户端进行 UI 原地替换盖图
						Client_NotifyWeaponEvolved(WeaponToSacrificeTag, EvolvedTag);
						
						UE_LOG(LogTemp, Warning, TEXT("Evolution Executed! %s sacrificed to become %s"), *WeaponToSacrificeTag.ToString(), *EvolvedTag.ToString());
					}
				}
			}
		}
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

void UVS_AbilitySystemComponent::AddHistoryWeapon(const FGameplayTag& WeaponTag)
{
	if (!WeaponTag.IsValid()) return;
	EvolvedHistoryWeapons.AddUnique(WeaponTag);
}

void UVS_AbilitySystemComponent::Client_NotifyWeaponEvolved_Implementation(const FGameplayTag& OldTag, const FGameplayTag& NewTag)
{
	// 通知客户端：武器发生了进化！
	// 后续会在 WidgetController 中通过这个调用触发 UI 更新
	OnWeaponEvolved.Broadcast(OldTag, NewTag);
}

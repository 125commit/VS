#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/VS_AbilitySystemComponent.h"
#include "AbilitySystem/VS_AttributeSet.h"
#include "Player/VS_PlayerState.h"

void UVS_OverlayWidgetController::BroadcastInitialValues()
{
	if (const UVS_AttributeSet* VSAS = GetVSAS())
	{
		OnHealthChanged.Broadcast(VSAS->GetHealth());
		OnMaxHealthChanged.Broadcast(VSAS->GetMaxHealth());
	}

	if (const AVS_PlayerState* VSPS = GetVSPS())
	{
		OnPlayerLevelChangedDelegate.Broadcast(VSPS->GetPlayerLevel());
		OnKillCountChangedDelegate.Broadcast(VSPS->GetKillCount());
		OnGoldChangedDelegate.Broadcast(VSPS->GetGold());
	}
}

void UVS_OverlayWidgetController::BindCallbacksToDependencies()
{
	if (AVS_PlayerState* VSPS = GetVSPS())
	{
		VSPS->OnXPPercentChangedDelegate.AddLambda(
			[this](const float NewValue)
			{
				OnXPPercentChangedDelegate.Broadcast(NewValue);
			}
		);

		VSPS->OnXPWindowChangedDelegate.AddLambda(
			[this](const float XPThisLevel, const float XPRequirement)
			{
				OnXPWindowChangedDelegate.Broadcast(XPThisLevel, XPRequirement);
			}
		);

		VSPS->OnLevelChangedDelegate.AddLambda(
			[this](const int32 NewLevel)
			{
				OnPlayerLevelChangedDelegate.Broadcast(NewLevel);
			}
		);

		VSPS->OnKillCountChangedDelegate.AddLambda(
			[this](const int32 NewValue)
			{
				OnKillCountChangedDelegate.Broadcast(NewValue);
			}
		);

		VSPS->OnGoldChangedDelegate.AddLambda(
			[this](const int32 NewValue)
			{
				OnGoldChangedDelegate.Broadcast(NewValue);
			}
		);
	}

	if (AbilitySystemComponent && GetVSAS())
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetVSAS()->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(GetVSAS()->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
	}

	if (UVS_AbilitySystemComponent* VSASC = GetVSASC())
	{
		if (VSASC->bStartupAbilitiesGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			VSASC->AbilitiesGivenDelegate.AddUObject(this, &UVS_OverlayWidgetController::BroadcastAbilityInfo);
		}

		VSASC->EffectAssetTags.AddUObject(this, &UVS_OverlayWidgetController::BroadcastEffectAssetTags);
	}
}

void UVS_OverlayWidgetController::BroadcastEffectAssetTags(const FGameplayTagContainer& AssetTags)
{
	if (!MessageWidgetDataTable)
	{
		return;
	}

	const FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"), false);
	if (!MessageTag.IsValid())
	{
		return;
	}

	for (const FGameplayTag& Tag : AssetTags)
	{
		if (Tag.MatchesTag(MessageTag))
		{
			if (const FVSUIWidgetRow* Row = GetDataTableRowByTag<FVSUIWidgetRow>(MessageWidgetDataTable, Tag))
			{
				MessageWidgetRowDelegate.Broadcast(*Row);
			}
		}
	}
}

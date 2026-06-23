#include "UI/WidgetController/VS_WidgetController.h"

#include "AbilitySystem/VS_AbilitySystemComponent.h"
#include "AbilitySystem/VS_AttributeSet.h"
#include "Data/VSAbilityInfoData.h"
#include "Player/VS_PlayerController.h"
#include "Player/VS_PlayerState.h"

void UVS_WidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UVS_WidgetController::BroadcastInitialValues()
{
	
}

void UVS_WidgetController::BindCallbacksToDependencies()
{
	if (UVS_AbilitySystemComponent* VSASC = GetVSASC())
	{
		VSASC->OnWeaponEvolved.AddDynamic(this, &UVS_WidgetController::WeaponEvolvedDelegate_Broadcast);
	}
}

void UVS_WidgetController::WeaponEvolvedDelegate_Broadcast(const FGameplayTag& OldTag, const FGameplayTag& NewTag)
{
	WeaponEvolvedDelegate.Broadcast(OldTag, NewTag);
}

void UVS_WidgetController::BroadcastAbilityInfo()
{
	if (!AbilityInfo)
	{
		return;
	}

	// TODO: Broadcast granted ability data after UVS_AbilitySystemComponent exposes ability-spec tag helpers.
}

AVS_PlayerController* UVS_WidgetController::GetVSPC()
{
	if (VSPlayerController == nullptr)
	{
		VSPlayerController = Cast<AVS_PlayerController>(PlayerController);
	}
	return VSPlayerController;
}

AVS_PlayerState* UVS_WidgetController::GetVSPS()
{
	if (VSPlayerState == nullptr)
	{
		VSPlayerState = Cast<AVS_PlayerState>(PlayerState);
	}
	return VSPlayerState;
}

UVS_AbilitySystemComponent* UVS_WidgetController::GetVSASC()
{
	if (VSAbilitySystemComponent == nullptr)
	{
		VSAbilitySystemComponent = Cast<UVS_AbilitySystemComponent>(AbilitySystemComponent);
	}
	return VSAbilitySystemComponent;
}

UVS_AttributeSet* UVS_WidgetController::GetVSAS()
{
	if (VSAttributeSet == nullptr)
	{
		VSAttributeSet = Cast<UVS_AttributeSet>(AttributeSet);
	}
	return VSAttributeSet;
}


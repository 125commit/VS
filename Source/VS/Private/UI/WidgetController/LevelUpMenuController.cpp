#include "UI/WidgetController/LevelUpMenuController.h"

#include "AbilitySystem/VS_AbilitySystemComponent.h"
#include "Player/VS_PlayerController.h"

void ULevelUpMenuController::BroadcastInitialValues()
{
	// WidgetControllerSet can happen after SetSkillOptions, so replay cached data for late-bound widgets.
	if (CachedCardInfos.Num() > 0)
	{
		OnLevelUpOptionsChanged.Broadcast(CachedCardInfos);
	}
}

void ULevelUpMenuController::BindCallbacksToDependencies()
{
	if (AVS_PlayerController* VSPC = GetVSPC())
	{
		// PlayerController owns the level-up roll; this controller translates it into UI card data.
		VSPC->OnShowLevelUpMenuDelegate.AddUObject(this, &ULevelUpMenuController::SetSkillOptions);
	}
}

void ULevelUpMenuController::SetSkillOptions(const TArray<FVSAbilityInfo>& InOptions)
{
	CachedCardInfos.Empty();

	for (const FVSAbilityInfo& OptionInfo : InOptions)
	{
		CachedCardInfos.Add(BuildCardInfo(OptionInfo));
	}

	OnLevelUpOptionsChanged.Broadcast(CachedCardInfos);
}

void ULevelUpMenuController::SelectUpgrade(FGameplayTag SelectedTag)
{
	if (!SelectedTag.IsValid())
	{
		return;
	}

	if (AVS_PlayerController* VSPC = GetVSPC())
	{
		// The server must own the actual grant/upgrade so the UI cannot spoof ability state.
		VSPC->Server_SelectUpgrade(SelectedTag);
	}
}

void ULevelUpMenuController::RerollUpgrade()
{
	if (AVS_PlayerController* VSPC = GetVSPC())
	{
		VSPC->Server_RerollUpgrade();
	}
}

FVSLevelUpCardInfo ULevelUpMenuController::BuildCardInfo(const FVSAbilityInfo& InAbilityInfo)
{
	FVSLevelUpCardInfo CardInfo;
	CardInfo.AbilityInfo = InAbilityInfo;
	CardInfo.AbilityTag = InAbilityInfo.AbilityTag;
	CardInfo.CurrentLevel = GetCurrentLevelForAbilityTag(InAbilityInfo.AbilityTag);
	CardInfo.NextLevel = FMath::Clamp(CardInfo.CurrentLevel + 1, 1, InAbilityInfo.MaxLevel);
	CardInfo.bIsNewAbility = CardInfo.CurrentLevel <= 0;
	CardInfo.LevelDescription = GetDescriptionForLevel(InAbilityInfo, CardInfo.NextLevel);

	return CardInfo;
}

int32 ULevelUpMenuController::GetCurrentLevelForAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const UVS_AbilitySystemComponent* VSASC = GetVSASC())
	{
		for (const FVSOwnedAbilityInfo& OwnedInfo : VSASC->GetOwnedAbilities())
		{
			if (OwnedInfo.AbilityTag.MatchesTagExact(AbilityTag))
			{
				return OwnedInfo.Level;
			}
		}
	}

	return 0;
}

FText ULevelUpMenuController::GetDescriptionForLevel(const FVSAbilityInfo& InAbilityInfo, int32 Level)
{
	if (InAbilityInfo.LevelDescriptions.Num() <= 0)
	{
		return FText::GetEmpty();
	}

	// Level is 1-based, while the description array is 0-based.
	const int32 DescriptionIndex = FMath::Clamp(Level - 1, 0, InAbilityInfo.LevelDescriptions.Num() - 1);
	return InAbilityInfo.LevelDescriptions[DescriptionIndex];
}

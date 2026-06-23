#include "UI/WidgetController/LevelUpMenuController.h"

#include "AbilitySystem/VS_AbilitySystemComponent.h"
#include "Player/VS_PlayerController.h"
#include "Player/VS_PlayerState.h"

void ULevelUpMenuController::BroadcastInitialValues()
{
	// WidgetControllerSet can happen after SetSkillOptions, so replay cached data for late-bound widgets.
	if (CachedCardInfos.Num() > 0)
	{
		OnLevelUpOptionsChanged.Broadcast(CachedCardInfos);
	}
	BroadcastRerollCount();
}

void ULevelUpMenuController::BindCallbacksToDependencies()
{
	// VS_HUD is the only listener for the player controller's level-up broadcast.
	// Keeping this controller unbound here prevents reroll from rebroadcasting the same cards twice.
}

void ULevelUpMenuController::SetSkillOptions(const TArray<FVSAbilityInfo>& InOptions)
{
	CachedCardInfos.Empty();

	for (const FVSAbilityInfo& OptionInfo : InOptions)
	{
		CachedCardInfos.Add(BuildCardInfo(OptionInfo));
	}

	UE_LOG(LogTemp, Warning, TEXT("LevelUpMenuController broadcasting %d card infos."), CachedCardInfos.Num());
	OnLevelUpOptionsChanged.Broadcast(CachedCardInfos);
	BroadcastRerollCount();
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
	BroadcastRerollCount();
}

int32 ULevelUpMenuController::GetRemainingRerollCount()
{
	if (AVS_PlayerState* VSPS = GetVSPS())
	{
		return VSPS->GetRerollCount();
	}

	return 0;
}

void ULevelUpMenuController::BroadcastRerollCount()
{
	// PlayerState owns the count; this controller only exposes it to the level-up widget.
	OnRerollCountChanged.Broadcast(GetRemainingRerollCount());
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

	// 预测是否触发进化
	if (AVS_PlayerController* VSPC = GetVSPC())
	{
		if (VSPC->AbilityInfoData)
		{
			FGameplayTag EvolvedTag = VSPC->AbilityInfoData->CheckIfCausesEvolution(InAbilityInfo.AbilityTag, CardInfo.NextLevel, GetVSASC());
			if (EvolvedTag.IsValid())
			{
				CardInfo.bWillCauseEvolution = true;
				CardInfo.EvolvedAbilityInfo = VSPC->AbilityInfoData->FindAbilityInfoForTag(EvolvedTag);
				UE_LOG(LogTemp, Warning, TEXT("Level-up card %s WILL CAUSE EVOLUTION to %s!"), *CardInfo.AbilityTag.ToString(), *EvolvedTag.ToString());
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Level-up card %s current level %d next level %d."),
		*CardInfo.AbilityTag.ToString(), CardInfo.CurrentLevel, CardInfo.NextLevel);

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
	if (!InAbilityInfo.LevelTable)
	{
		return FText::GetEmpty();
	}

	TArray<FVSAbilityLevelRow*> AllRows;
	InAbilityInfo.LevelTable->GetAllRows<FVSAbilityLevelRow>(TEXT("GetDescriptionForLevel"), AllRows);

	if (AllRows.Num() <= 0)
	{
		return FText::GetEmpty();
	}

	// Level is 1-based, while the description array is 0-based.
	const int32 DescriptionIndex = FMath::Clamp(Level - 1, 0, AllRows.Num() - 1);
	if (AllRows.IsValidIndex(DescriptionIndex) && AllRows[DescriptionIndex])
	{
		return AllRows[DescriptionIndex]->LevelDescription;
	}

	return FText::GetEmpty();
}

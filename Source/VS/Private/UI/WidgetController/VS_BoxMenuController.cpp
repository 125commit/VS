#include "UI/WidgetController/VS_BoxMenuController.h"

void UVS_BoxMenuController::BroadcastInitialValues()
{
	OnChestRewardChanged.Broadcast(CachedRewardInfo);
}

void UVS_BoxMenuController::SetChestData(int32 GoldAmount, const FVSAbilityInfo& AwardedSkill, bool bIsEvolution, const FVSAbilityInfo& EvolvedSkill)
{
	SavedGoldAmount = GoldAmount;
	SavedAwardedSkill = AwardedSkill;
	bSavedHasAwardedSkill = AwardedSkill.AbilityTag.IsValid();
	bSavedIsEvolution = bIsEvolution;
	SavedEvolvedSkill = EvolvedSkill;

	// Cache one complete payload so WBP_BoxScreen can update text, icon, and evolution state from a single event.
	CachedRewardInfo.GoldAmount = SavedGoldAmount;
	CachedRewardInfo.bHasAwardedSkill = bSavedHasAwardedSkill;
	CachedRewardInfo.AwardedSkill = SavedAwardedSkill;
	CachedRewardInfo.bIsEvolution = SavedEvolvedSkill.AbilityTag.IsValid() && bSavedIsEvolution;
	CachedRewardInfo.EvolvedSkill = SavedEvolvedSkill;
}

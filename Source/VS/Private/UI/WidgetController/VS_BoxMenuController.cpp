#include "UI/WidgetController/VS_BoxMenuController.h"

void UVS_BoxMenuController::SetChestData(int32 GoldAmount, const FVSAbilityInfo& AwardedSkill, bool bIsEvolution, const FVSAbilityInfo& EvolvedSkill)
{
	SavedGoldAmount = GoldAmount;
	SavedAwardedSkill = AwardedSkill;
	bSavedIsEvolution = bIsEvolution;
	SavedEvolvedSkill = EvolvedSkill;
}

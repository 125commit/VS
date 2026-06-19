#include "UI/WidgetController/VS_BoxMenuController.h"

void UVS_BoxMenuController::SetChestData(int32 GoldAmount, const FVSAbilityInfo& AwardedSkill)
{
	SavedGoldAmount = GoldAmount;
	SavedAwardedSkill = AwardedSkill;
}

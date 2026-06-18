#include "UI/WidgetController/VS_LevelUpMenuController.h"

void UVS_LevelUpMenuController::SetSkillOptions(const TArray<FVSAbilityInfo>& InOptions)
{
	SavedSkillOptions = InOptions;
}

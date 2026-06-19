#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/VS_WidgetController.h"
#include "Data/DA_AbilityInfo.h"
#include "VS_LevelUpMenuController.generated.h"

/**
 * 升级菜单控制器
 */
UCLASS(BlueprintType, Blueprintable)
class VS_API UVS_LevelUpMenuController : public UVS_WidgetController
{
	GENERATED_BODY()

public:
	// 由 HUD调用的数据注入接口
	void SetSkillOptions(const TArray<FVSAbilityInfo>& InOptions);

protected:
	TArray<FVSAbilityInfo> SavedSkillOptions;
};

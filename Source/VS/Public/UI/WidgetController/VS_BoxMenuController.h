#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/VS_WidgetController.h"
#include "Data/VSAbilityInfoData.h"
#include "VS_BoxMenuController.generated.h"

/**
 * 宝箱菜单控制器
 */
UCLASS(BlueprintType, Blueprintable)
class VS_API UVS_BoxMenuController : public UVS_WidgetController
{
	GENERATED_BODY()

public:
	// 由 HUD调用的数据注入接口
	void SetChestData(int32 GoldAmount, const FVSAbilityInfo& AwardedSkill, bool bIsEvolution = false, const FVSAbilityInfo& EvolvedSkill = FVSAbilityInfo());

protected:
	int32 SavedGoldAmount;
	FVSAbilityInfo SavedAwardedSkill;

	UPROPERTY(BlueprintReadOnly)
	bool bSavedIsEvolution;

	UPROPERTY(BlueprintReadOnly)
	FVSAbilityInfo SavedEvolvedSkill;
};

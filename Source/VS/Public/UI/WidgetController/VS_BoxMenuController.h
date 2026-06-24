#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/VS_WidgetController.h"
#include "Data/VSAbilityInfoData.h"
#include "VS_BoxMenuController.generated.h"

/**
 * 宝箱菜单控制器
 */
USTRUCT(BlueprintType)
struct FVSChestRewardInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "VS|Chest")
	int32 GoldAmount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "VS|Chest")
	bool bHasAwardedSkill = false;

	UPROPERTY(BlueprintReadOnly, Category = "VS|Chest")
	FVSAbilityInfo AwardedSkill;

	UPROPERTY(BlueprintReadOnly, Category = "VS|Chest")
	bool bIsEvolution = false;

	UPROPERTY(BlueprintReadOnly, Category = "VS|Chest")
	FVSAbilityInfo EvolvedSkill;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVSChestRewardChangedSignature, const FVSChestRewardInfo&, RewardInfo);

UCLASS(BlueprintType, Blueprintable)
class VS_API UVS_BoxMenuController : public UVS_WidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;

	// HUD injects the chest reward payload before WBP_BoxScreen renders it.
	UFUNCTION(BlueprintCallable, Category = "VS|Chest")
	void SetChestData(int32 GoldAmount, const FVSAbilityInfo& AwardedSkill, bool bIsEvolution = false, const FVSAbilityInfo& EvolvedSkill = FVSAbilityInfo());

	UPROPERTY(BlueprintAssignable, Category = "VS|Chest")
	FVSChestRewardChangedSignature OnChestRewardChanged;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "VS|Chest")
	int32 SavedGoldAmount = 0;

	UPROPERTY(BlueprintReadOnly, Category = "VS|Chest")
	FVSAbilityInfo SavedAwardedSkill;

	UPROPERTY(BlueprintReadOnly, Category = "VS|Chest")
	bool bSavedHasAwardedSkill = false;

	UPROPERTY(BlueprintReadOnly, Category = "VS|Chest")
	bool bSavedIsEvolution = false;

	UPROPERTY(BlueprintReadOnly, Category = "VS|Chest")
	FVSAbilityInfo SavedEvolvedSkill;

	UPROPERTY(BlueprintReadOnly, Category = "VS|Chest")
	FVSChestRewardInfo CachedRewardInfo;
};

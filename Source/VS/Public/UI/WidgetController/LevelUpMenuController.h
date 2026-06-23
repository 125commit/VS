#pragma once

#include "CoreMinimal.h"
#include "Data/VSAbilityInfoData.h"
#include "UI/WidgetController/VS_WidgetController.h"
#include "LevelUpMenuController.generated.h"

// Runtime data for one level-up card.
// FVSAbilityInfo is static data; level and description are derived from the player's current ASC state.
USTRUCT(BlueprintType)
struct FVSLevelUpCardInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FVSAbilityInfo AbilityInfo;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag AbilityTag;

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentLevel = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 NextLevel = 1;

	UPROPERTY(BlueprintReadOnly)
	FText LevelDescription;

	UPROPERTY(BlueprintReadOnly)
	bool bIsNewAbility = true;

	UPROPERTY(BlueprintReadOnly)
	bool bWillCauseEvolution = false;

	UPROPERTY(BlueprintReadOnly)
	FVSAbilityInfo EvolvedAbilityInfo;
};

// Broadcasts the latest level-up card list to WBP_LevelUpScreen.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVSLevelUpOptionsChangedSignature, const TArray<FVSLevelUpCardInfo>&, CardInfos);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVSRerollCountChangedSignature, int32, RerollCount);

UCLASS(BlueprintType, Blueprintable)
class VS_API ULevelUpMenuController : public UVS_WidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	// Receives this roll's options, enriches them with level text, then broadcasts to UI.
	UFUNCTION(BlueprintCallable, Category = "VS|LevelUp")
	void SetSkillOptions(const TArray<FVSAbilityInfo>& InOptions);

	// Called by the UI after a skill card is clicked. Forwards the selected tag to the server.
	UFUNCTION(BlueprintCallable, Category = "VS|LevelUp")
	void SelectUpgrade(FGameplayTag SelectedTag);

	// Called by the level-up screen reroll button.
	UFUNCTION(BlueprintCallable, Category = "VS|LevelUp")
	void RerollUpgrade();

	UPROPERTY(BlueprintAssignable, Category = "VS|LevelUp")
	FVSLevelUpOptionsChangedSignature OnLevelUpOptionsChanged;

	UPROPERTY(BlueprintAssignable, Category = "VS|LevelUp")
	FVSRerollCountChangedSignature OnRerollCountChanged;

	UFUNCTION(BlueprintPure, Category = "VS|LevelUp")
	int32 GetRemainingRerollCount();

protected:
	// Cache the latest generated card data so late-bound widgets can request an immediate replay.
	UPROPERTY(BlueprintReadOnly, Category = "VS|LevelUp")
	TArray<FVSLevelUpCardInfo> CachedCardInfos;

private:
	void BroadcastRerollCount();
	FVSLevelUpCardInfo BuildCardInfo(const FVSAbilityInfo& InAbilityInfo);
	int32 GetCurrentLevelForAbilityTag(const FGameplayTag& AbilityTag);
	FText GetDescriptionForLevel(const FVSAbilityInfo& InAbilityInfo, int32 Level);
};

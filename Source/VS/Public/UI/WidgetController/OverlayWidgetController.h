#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/VS_WidgetController.h"
#include "OverlayWidgetController.generated.h"

class UVS_UserWidget;

USTRUCT(BlueprintType)
struct FVSUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UVS_UserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Image = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVSOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVSOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FVSOnXPWindowChangedSignature, float, XPThisLevel, float, XPRequirement);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVSMessageWidgetRowSignature, FVSUIWidgetRow, Row);

UCLASS(BlueprintType, Blueprintable)
class VS_API UVS_OverlayWidgetController : public UVS_WidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FVSOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FVSOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
	FVSMessageWidgetRowSignature MessageWidgetRowDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|XP")
	FVSOnAttributeChangedSignature OnXPPercentChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|XP")
	FVSOnXPWindowChangedSignature OnXPWindowChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Level")
	FVSOnPlayerStatChangedSignature OnPlayerLevelChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Stats")
	FVSOnPlayerStatChangedSignature OnKillCountChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Stats")
	FVSOnPlayerStatChangedSignature OnGoldChangedDelegate;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;

	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);

private:
	void BroadcastEffectAssetTags(const FGameplayTagContainer& AssetTags);
};

template <typename T>
T* UVS_OverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
}

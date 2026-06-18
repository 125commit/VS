#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "GameplayTagContainer.h"
#include "Data/DA_ToolInfo.h"
#include "VS_GameMode.generated.h"

class UDA_ToolInfo;
class AVS_PlayerController;

UCLASS()
class VS_API AVS_GameMode : public AGameMode
{
	GENERATED_BODY()

public:

	// 策划配表：在蓝图 GM 中选择配好的 DA_ToolInfo 资产
	UPROPERTY(EditDefaultsOnly, Category = "VS|Store")
	TObjectPtr<UDA_ToolInfo> ToolPriceData;

	// ==========================================================
	// PC 交互接口：主菜单与购买
	// ==========================================================

	// 被主菜单的 PC 调用以开始游戏
	UFUNCTION(BlueprintCallable, Category = "VS|GameFlow")
	void TravelToGameMap();

	// 被主菜单的 PC 调用以购买天赋
	UFUNCTION(BlueprintCallable, Category = "VS|GameFlow")
	bool ProcessItemPurchase(FGameplayTag ItemTag, AVS_PlayerController* PC);

	// ==========================================================
	// PC 交互接口：战斗结束与结算
	// ==========================================================

	// 被结算屏幕退出按钮调用，或者玩家主动退出时调用
	UFUNCTION(BlueprintCallable, Category = "VS|GameFlow")
	void TravelToMainMenuMap();

	// 玩家战死或达成条件胜利后，由 GM 主动执行数据剥壳，并跳转结算
	UFUNCTION(BlueprintCallable, Category = "VS|GameFlow")
	void ProcessMatchEndAndTravel(AVS_PlayerController* PC);

};
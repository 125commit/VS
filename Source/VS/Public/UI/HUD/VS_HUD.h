#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Data/DA_AbilityInfo.h"
#include "VS_HUD.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class UVS_OverlayWidgetController;
class UVS_UserWidget;
struct FWidgetControllerParams;

UCLASS()
class VS_API AVS_HUD : public AHUD
{
	GENERATED_BODY()

public:
	UVS_OverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	class UVS_LevelUpMenuController* GetLevelUpMenuController(const FWidgetControllerParams& WCParams);
	class UVS_BoxMenuController* GetBoxMenuController(const FWidgetControllerParams& WCParams);
	class UVS_PauseMenuController* GetPauseMenuController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:
	// C++ 绑定的底层中继回调
	void OnShowLevelUpMenu(const TArray<FVSAbilityInfo>& SkillOptions);
	void OnShowChestMenu(int32 GoldAmount, const FVSAbilityInfo& AwardedSkill);
	void OnShowPauseMenu();

	UPROPERTY()
	TObjectPtr<UVS_UserWidget> OverlayWidget;	

	UPROPERTY(EditAnywhere, Category = "VS|UI")
	TSubclassOf<UVS_UserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UVS_OverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere, Category = "VS|UI")
	TSubclassOf<UVS_OverlayWidgetController> OverlayWidgetControllerClass;

	// ==========================================================
	// 懒加载实例化：各种弹窗菜单的 Widget 和 Controller
	// ==========================================================

	UPROPERTY()
	TObjectPtr<UVS_UserWidget> LevelUpWidget;

	UPROPERTY(EditAnywhere, Category = "VS|UI")
	TSubclassOf<UVS_UserWidget> LevelUpWidgetClass;

	UPROPERTY()
	TObjectPtr<UVS_LevelUpMenuController> LevelUpMenuController;

	UPROPERTY(EditAnywhere, Category = "VS|UI")
	TSubclassOf<UVS_LevelUpMenuController> LevelUpMenuControllerClass;

	UPROPERTY()
	TObjectPtr<UVS_UserWidget> BoxWidget;

	UPROPERTY(EditAnywhere, Category = "VS|UI")
	TSubclassOf<UVS_UserWidget> BoxWidgetClass;

	UPROPERTY()
	TObjectPtr<UVS_BoxMenuController> BoxMenuController;

	UPROPERTY(EditAnywhere, Category = "VS|UI")
	TSubclassOf<UVS_BoxMenuController> BoxMenuControllerClass;

	UPROPERTY()
	TObjectPtr<UVS_UserWidget> PauseWidget;

	UPROPERTY(EditAnywhere, Category = "VS|UI")
	TSubclassOf<UVS_UserWidget> PauseWidgetClass;

	UPROPERTY()
	TObjectPtr<UVS_PauseMenuController> PauseMenuController;

	UPROPERTY(EditAnywhere, Category = "VS|UI")
	TSubclassOf<UVS_PauseMenuController> PauseMenuControllerClass;
};

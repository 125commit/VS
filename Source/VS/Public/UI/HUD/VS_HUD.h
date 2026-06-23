#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Data/VSAbilityInfoData.h"
#include "VS_HUD.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;
class UVS_OverlayWidgetController;
class ULevelUpMenuController;
class UVS_UserWidget;
struct FWidgetControllerParams;

UCLASS()
class VS_API AVS_HUD : public AHUD
{
	GENERATED_BODY()

public:
	AVS_HUD();

	UVS_OverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	ULevelUpMenuController* GetLevelUpMenuController(const FWidgetControllerParams& WCParams);
	class UVS_BoxMenuController* GetBoxMenuController(const FWidgetControllerParams& WCParams);
	class UVS_PauseMenuController* GetPauseMenuController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

protected:
	virtual void BeginPlay() override;

private:
	// C++ 绑定的底层中继回调
	void BindPlayerControllerDelegates(APlayerController* PC);
	void OnShowLevelUpMenu(const TArray<FVSAbilityInfo>& SkillOptions);
	void OnShowChestMenu(int32 GoldAmount, const FVSAbilityInfo& AwardedSkill, bool bIsEvolution = false, const FVSAbilityInfo& EvolvedSkillInfo = FVSAbilityInfo());
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

	bool bHasBoundPlayerControllerDelegates = false;

	UPROPERTY()
	TObjectPtr<ULevelUpMenuController> LevelUpMenuController;

	UPROPERTY(EditAnywhere, Category = "VS|UI")
	TSubclassOf<ULevelUpMenuController> LevelUpMenuControllerClass;

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

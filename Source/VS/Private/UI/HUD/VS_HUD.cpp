#include "UI/HUD/VS_HUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/Widget/VS_UserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "Player/VS_PlayerController.h"
#include "UObject/ConstructorHelpers.h"

#include "UI/WidgetController/LevelUpMenuController.h"
#include "UI/WidgetController/VS_BoxMenuController.h"
#include "UI/WidgetController/VS_PauseMenuController.h"

AVS_HUD::AVS_HUD()
{
	LevelUpMenuControllerClass = ULevelUpMenuController::StaticClass();

	// The project currently uses VS_HUD directly, so provide the level-up widget class from C++.
	static ConstructorHelpers::FClassFinder<UVS_UserWidget> LevelUpWidgetBP(TEXT("/Game/Blueprint/UI/WBP_LevelUpScreen"));
	if (LevelUpWidgetBP.Succeeded())
	{
		LevelUpWidgetClass = LevelUpWidgetBP.Class;
	}
}

void AVS_HUD::BeginPlay()
{
	Super::BeginPlay();

	BindPlayerControllerDelegates(GetOwningPlayerController());
}

void AVS_HUD::BindPlayerControllerDelegates(APlayerController* PC)
{
	if (bHasBoundPlayerControllerDelegates)
	{
		return;
	}

	if (AVS_PlayerController* VSPC = Cast<AVS_PlayerController>(PC))
	{
		// Level-up/chest/pause are broadcast by the PlayerController; HUD owns the matching pop-up widgets.
		VSPC->OnShowLevelUpMenuDelegate.AddUObject(this, &AVS_HUD::OnShowLevelUpMenu);
		VSPC->OnShowChestMenuDelegate.AddUObject(this, &AVS_HUD::OnShowChestMenu);
		VSPC->OnShowPauseMenuDelegate.AddUObject(this, &AVS_HUD::OnShowPauseMenu);

		bHasBoundPlayerControllerDelegates = true;
	}
}

UVS_OverlayWidgetController* AVS_HUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UVS_OverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

ULevelUpMenuController* AVS_HUD::GetLevelUpMenuController(const FWidgetControllerParams& WCParams)
{
	if (LevelUpMenuController == nullptr)
	{
		LevelUpMenuController = NewObject<ULevelUpMenuController>(this, LevelUpMenuControllerClass);
		LevelUpMenuController->SetWidgetControllerParams(WCParams);
		LevelUpMenuController->BindCallbacksToDependencies();
	}
	return LevelUpMenuController;
}

UVS_BoxMenuController* AVS_HUD::GetBoxMenuController(const FWidgetControllerParams& WCParams)
{
	if (BoxMenuController == nullptr)
	{
		BoxMenuController = NewObject<UVS_BoxMenuController>(this, BoxMenuControllerClass);
		BoxMenuController->SetWidgetControllerParams(WCParams);
		BoxMenuController->BindCallbacksToDependencies();
	}
	return BoxMenuController;
}

UVS_PauseMenuController* AVS_HUD::GetPauseMenuController(const FWidgetControllerParams& WCParams)
{
	if (PauseMenuController == nullptr)
	{
		PauseMenuController = NewObject<UVS_PauseMenuController>(this, PauseMenuControllerClass);
		PauseMenuController->SetWidgetControllerParams(WCParams);
		PauseMenuController->BindCallbacksToDependencies();
	}
	return PauseMenuController;
}

void AVS_HUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_VS_HUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_VS_HUD"));
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UVS_UserWidget>(Widget);
	
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UVS_OverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	Widget->AddToViewport();

	// ==========================================================
	// 绑定 PC 的三大原生广播
	// ==========================================================
	BindPlayerControllerDelegates(PC);
}

void AVS_HUD::OnShowLevelUpMenu(const TArray<FVSAbilityInfo>& SkillOptions)
{
	checkf(LevelUpWidgetClass, TEXT("LevelUp Widget Class uninitialized, please fill out BP_VS_HUD"));
	checkf(LevelUpMenuControllerClass, TEXT("LevelUp Menu Controller Class uninitialized, please fill out BP_VS_HUD"));
	UE_LOG(LogTemp, Warning, TEXT("VS_HUD opening level-up menu with %d skill options."), SkillOptions.Num());

	const bool bCreatedLevelUpWidget = LevelUpWidget == nullptr;
	if (LevelUpWidget == nullptr)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), LevelUpWidgetClass);
		LevelUpWidget = Cast<UVS_UserWidget>(Widget);
	}

	if (LevelUpWidget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("VS_HUD failed to create WBP_LevelUpScreen. Check widget path and parent class."));
		return;
	}

	APlayerController* PC = GetOwningPlayerController();
	APlayerState* PS = PC ? PC->PlayerState : nullptr;
	const FWidgetControllerParams WCParams(PC, PS, nullptr, nullptr);

	ULevelUpMenuController* WidgetController = GetLevelUpMenuController(WCParams);
	if (bCreatedLevelUpWidget)
	{
		// Give the widget its controller before broadcasting options so Blueprint event bindings are ready.
		LevelUpWidget->SetWidgetController(WidgetController);
	}
	WidgetController->SetSkillOptions(SkillOptions);
	WidgetController->BroadcastInitialValues();
	if (!LevelUpWidget->IsInViewport())
	{
		LevelUpWidget->AddToViewport();
	}
}

void AVS_HUD::OnShowChestMenu(int32 GoldAmount, const FVSAbilityInfo& AwardedSkill)
{
	checkf(BoxWidgetClass, TEXT("Box Widget Class uninitialized, please fill out BP_VS_HUD"));
	checkf(BoxMenuControllerClass, TEXT("Box Menu Controller Class uninitialized, please fill out BP_VS_HUD"));

	if (BoxWidget == nullptr)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), BoxWidgetClass);
		BoxWidget = Cast<UVS_UserWidget>(Widget);
	}

	APlayerController* PC = GetOwningPlayerController();
	APlayerState* PS = PC ? PC->PlayerState : nullptr;
	const FWidgetControllerParams WCParams(PC, PS, nullptr, nullptr);

	UVS_BoxMenuController* WidgetController = GetBoxMenuController(WCParams);
	WidgetController->SetChestData(GoldAmount, AwardedSkill);

	BoxWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	BoxWidget->AddToViewport();
}

void AVS_HUD::OnShowPauseMenu()
{
	checkf(PauseWidgetClass, TEXT("Pause Widget Class uninitialized, please fill out BP_VS_HUD"));
	checkf(PauseMenuControllerClass, TEXT("Pause Menu Controller Class uninitialized, please fill out BP_VS_HUD"));

	if (PauseWidget == nullptr)
	{
		UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), PauseWidgetClass);
		PauseWidget = Cast<UVS_UserWidget>(Widget);
	}

	APlayerController* PC = GetOwningPlayerController();
	APlayerState* PS = PC ? PC->PlayerState : nullptr;
	const FWidgetControllerParams WCParams(PC, PS, nullptr, nullptr);

	UVS_PauseMenuController* WidgetController = GetPauseMenuController(WCParams);

	PauseWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	PauseWidget->AddToViewport();
}

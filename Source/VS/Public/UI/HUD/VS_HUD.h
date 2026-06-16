#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
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

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:
	UPROPERTY()
	TObjectPtr<UVS_UserWidget> OverlayWidget;	

	UPROPERTY(EditAnywhere, Category = "VS|UI")
	TSubclassOf<UVS_UserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UVS_OverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere, Category = "VS|UI")
	TSubclassOf<UVS_OverlayWidgetController> OverlayWidgetControllerClass;
};

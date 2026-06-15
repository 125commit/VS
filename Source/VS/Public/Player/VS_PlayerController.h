#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "Data/DA_AbilityInfo.h" 
#include "VS_PlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UDA_AbilityInfo;

// ==========================================================
// 客户端喇叭：原生多播委托
// 传富文本的 FVSAbilityInfo 给 UI 选卡
// ==========================================================
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShowLevelUpMenuSignature, const TArray<FVSAbilityInfo>& /*SkillOptions*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnShowChestMenuSignature, int32 /*GoldAmount*/, const FVSAbilityInfo& /*AwardedSkill*/);
DECLARE_MULTICAST_DELEGATE(FOnShowPauseMenuSignature);

UCLASS()
class VS_API AVS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AVS_PlayerController();

	// 客户端大喇叭
	FOnShowLevelUpMenuSignature OnShowLevelUpMenuDelegate;
	FOnShowChestMenuSignature OnShowChestMenuDelegate;
	FOnShowPauseMenuSignature OnShowPauseMenuDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	// 全游戏技能图鉴，服务端查表抽卡，客户端查表翻译
	UPROPERTY(EditDefaultsOnly, Category = "VS|Data")
	TObjectPtr<UDA_AbilityInfo> AbilityInfoData;

	// 输入管线
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> VSContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> PauseAction;

	void Move(const FInputActionValue& InputActionValue);
	void PauseButtonPressed();

public:
	// ==========================================================
	// 服务端黑盒权威 (Server RPCs)
	// ==========================================================

	// 【局内：暂停管线】
	UFUNCTION(Server, Reliable)
	void Server_PauseGame();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="VS|GameFlow")
	void Server_ResumeGame();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="VS|GameFlow")
	void Server_QuitToSettlement();

	// 【局内：升级与重抽管线】
	UFUNCTION(Server, Reliable)
	void Server_HandleLevelUp();

	// 暴露给UI调用：玩家点击“重新Roll”
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="VS|LevelUp")
	void Server_RerollUpgrade();

	// 暴露给UI调用：玩家确定选择了技能
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="VS|LevelUp")
	void Server_SelectUpgrade(FGameplayTag SelectedTag);

	// 【局内：宝箱管线】
	UFUNCTION(Server, Reliable)
	void Server_ProcessChestPickup();

	// 【局外管线】
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="VS|MainMenu")
	void Server_AcceptSettlement();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="VS|MainMenu")
	void Server_BuyItem(FGameplayTag ItemTag);

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="VS|MainMenu")
	void Server_StartGame();

	// ==========================================================
	// 客户端接收端 (Client RPCs)
	// ==========================================================
	// 剔除了 Level 和 XP，只下发 Tag
	UFUNCTION(Client, Reliable)
	void Client_ShowLevelUpScreen(const TArray<FGameplayTag>& SkillOptions);

	UFUNCTION(Client, Reliable)
	void Client_ShowChestScreen(int32 GoldAmount, FGameplayTag AwardedTag);
};
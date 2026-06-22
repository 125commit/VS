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

	FOnShowLevelUpMenuSignature OnShowLevelUpMenuDelegate;
	FOnShowChestMenuSignature OnShowChestMenuDelegate;
	FOnShowPauseMenuSignature OnShowPauseMenuDelegate;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	// 全游戏技能图鉴
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

	// ==========================================================
	// 辅助抽卡算法：校验 ASC 槽位与等级，生成可用的发牌池
	// ==========================================================
	TArray<FGameplayTag> GenerateValidAbilityPool();

public:
	// ==========================================================
	// 服务端黑盒权威 (Server RPCs)
	// ==========================================================

	// 【局内：暂停管线】
	UFUNCTION(Server, Reliable)
	void Server_PauseGame();

	// 暴露给底层UI：暂停恢复、宝箱动画播完后均调用此函数解冻
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="VS|GameFlow")
	void Server_ResumeGame();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="VS|GameFlow")
	void Server_QuitToSettlement();

	// 【局内：升级管线】
	UFUNCTION(Server, Reliable)
	void Server_HandleLevelUp();

	// 【局内：重抽管线】
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="VS|LevelUp")
	void Server_RerollUpgrade();

	// 因为需要回传 Tag，所以不可与 ResumeGame 共用
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="VS|LevelUp")
	void Server_SelectUpgrade(FGameplayTag SelectedTag);

	// 【局内：宝箱管线】
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="VS|LevelUp")
	void Server_ProcessChestPickup(int32 MaxItems);

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
	UFUNCTION(Client, Reliable)
	void Client_ShowLevelUpScreen(const TArray<FGameplayTag>& SkillOptions);

	UFUNCTION(Client, Reliable)
	void Client_ShowChestScreen(int32 GoldAmount, FGameplayTag AwardedTag);
};
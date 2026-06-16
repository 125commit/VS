#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "VS_PlayerState.generated.h"

class UDA_LevelUpInfo;
class UAbilitySystemComponent;
class UVS_AttributeSet;

// -------------------------------------------------------------
// UI 与控制器广播委托 (Model -> Controller -> View 的通讯桥梁)
// -------------------------------------------------------------
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChanged, int32 /*StatValue*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerFloatStatChanged, float /*StatValue*/); // 用于浮点数，如进度条百分比
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnXPWindowChanged, float /*XPThisLevel*/, float /*XPRequirement*/); // 用于抛出本级经验文本

UCLASS()
class VS_API AVS_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AVS_PlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// -------------------------------------------------------------
	// GAS 核心组件接口
	// -------------------------------------------------------------
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UVS_AttributeSet* GetAttributeSet() const { return AttributeSet; }

	// -------------------------------------------------------------
	// 经验与等级广播委托 (供外部如 UI 控制器监听)
	// -------------------------------------------------------------
	FOnXPWindowChanged OnXPWindowChangedDelegate;         
	FOnPlayerStatChanged OnLevelChangedDelegate;          
	FOnPlayerFloatStatChanged OnXPPercentChangedDelegate; 
	FOnPlayerStatChanged OnKillCountChangedDelegate;
	FOnPlayerStatChanged OnGoldChangedDelegate;

	// -------------------------------------------------------------
	// 高性能 XP 结算系统 (彻底绕开 GAS，由 DropManager 调用)
	// -------------------------------------------------------------
	
	/* 接收基础经验，内部读取贪婪倍率后结算，并执行跨级循环验证 */
	UFUNCTION(BlueprintCallable, Category = "PlayerState|XP")
	void AddXP(float BaseXP);

	FORCEINLINE int32 GetPlayerLevel() const { return Level; }
	FORCEINLINE float GetXP() const { return XP; }
	FORCEINLINE int32 GetPendingLevelUps() const { return PendingLevelUps; }
	FORCEINLINE int32 GetKillCount() const { return KillCount; }
	FORCEINLINE int32 GetRerollCount() const { return RerollCount; }
	FORCEINLINE int32 GetGold() const { return Gold; }

	// 局内数据管理
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Stats")
	void AddKillCount(int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "PlayerState|Stats")
	void AddGold(int32 Amount);

	// 重抽与升级管理
	UFUNCTION(BlueprintCallable, Category = "PlayerState|Stats")
	void ConsumeRerollCount();

	UFUNCTION(BlueprintCallable, Category = "PlayerState|Stats")
	void ConsumePendingLevelUp();

protected:
	// -------------------------------------------------------------
	// 核心组件与数据资产
	// -------------------------------------------------------------
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UVS_AttributeSet> AttributeSet;

	// 升级数据表 (策划配置：每级所需经验)
	UPROPERTY(EditDefaultsOnly, Category = "LevelUp Data")
	TObjectPtr<UDA_LevelUpInfo> LevelUpInfo;

	// -------------------------------------------------------------
	// 状态数据 (需网络同步)
	// -------------------------------------------------------------
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level, Category = "PlayerState|Stats")
	int32 Level = 1;

	// 内部依然记录【绝对总经验】，因为只有绝对经验才绝对安全
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_XP, Category = "PlayerState|Stats")
	float XP = 0.f;

	// 待处理的升级次数 (每次升级积累一次，由控制器消费)
	UPROPERTY(VisibleAnywhere, Replicated, Category = "PlayerState|Stats")
	int32 PendingLevelUps = 0;

	// 可重抽次数 (默认为1)
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_RerollCount, Category = "PlayerState|Stats")
	int32 RerollCount = 1;

	// 单局击杀数
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_KillCount, Category = "PlayerState|Stats")
	int32 KillCount = 0;

	// 单局获取金币
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Gold, Category = "PlayerState|Stats")
	int32 Gold = 0;

	// -------------------------------------------------------------
	// 网络同步回调
	// -------------------------------------------------------------
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);

	UFUNCTION()
	void OnRep_XP(float OldXP);

	UFUNCTION()
	void OnRep_RerollCount(int32 OldRerollCount);

	UFUNCTION()
	void OnRep_KillCount(int32 OldKillCount);

	UFUNCTION()
	void OnRep_Gold(int32 OldGold);

private:
	// 辅助函数：统一计算并广播相对于【当前等级】的经验数值和百分比
	void CalculateAndBroadcastXPProgress();
};
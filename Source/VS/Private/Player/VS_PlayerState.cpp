#include "Player/VS_PlayerState.h"
#include "Player/VS_PlayerController.h" // 引入 PC
#include "AbilitySystem/VS_AbilitySystemComponent.h"
#include "AbilitySystem/VS_AttributeSet.h"
#include "Data/DA_LevelUpInfo.h"
#include "Net/UnrealNetwork.h"

AVS_PlayerState::AVS_PlayerState()
{
	SetNetUpdateFrequency(100.f);

	// 初始化等级为 1，防止开局 0 经验时瞬间触发升级导致整个游戏暂停死锁！
	Level = 1;
	XP = 0.f;

	AbilitySystemComponent = CreateDefaultSubobject<UVS_AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UVS_AttributeSet>("AttributeSet");
}

UAbilitySystemComponent* AVS_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AVS_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AVS_PlayerState, Level);
	DOREPLIFETIME(AVS_PlayerState, XP);
	DOREPLIFETIME(AVS_PlayerState, PendingLevelUps);
	DOREPLIFETIME(AVS_PlayerState, RerollCount);
	DOREPLIFETIME(AVS_PlayerState, KillCount);
	DOREPLIFETIME(AVS_PlayerState, Gold);
}
void AVS_PlayerState::AddXP(float BaseXP)
{
	if (!HasAuthority()) return;

	// 1. 读取贪婪倍率
	float GreedMultiplier = 1.0f;
	if (AttributeSet)
	{
		GreedMultiplier = AttributeSet->GetGreedMultiplier();
	}

	// 2. 累加绝对总经验
	XP += BaseXP * GreedMultiplier;

	// 3. 判定升级
	if (LevelUpInfo)
	{
		int32 TargetLevel = LevelUpInfo->FindLevelForXP(XP); 
		
		while (Level < TargetLevel)
		{
			Level++;
			PendingLevelUps++;
			
			OnLevelChangedDelegate.Broadcast(Level);
		}

		if (PendingLevelUps > 0)
		{
			// 通知 PlayerController 弹窗升级界面
			if (AVS_PlayerController* PC = Cast<AVS_PlayerController>(GetOwner()))
			{
				PC->Server_HandleLevelUp();
			}
		}
	}

	// 4. 计算并广播所有 UI 需要的经验数据（百分比 + 相对数值）
	CalculateAndBroadcastXPProgress();
}

void AVS_PlayerState::ConsumeRerollCount()
{
	if (HasAuthority() && RerollCount > 0)
	{
		RerollCount--;
	}
}

void AVS_PlayerState::ConsumePendingLevelUp()
{
	if (HasAuthority() && PendingLevelUps > 0)
	{
		PendingLevelUps--;
	}
}

void AVS_PlayerState::AddKillCount(int32 Amount)
{
	if (HasAuthority())
	{
		KillCount += Amount;
		OnKillCountChangedDelegate.Broadcast(KillCount);
	}
}

void AVS_PlayerState::AddGold(int32 Amount)
{
	if (HasAuthority())
	{
		Gold += Amount;
		OnGoldChangedDelegate.Broadcast(Gold);
	}
}

void AVS_PlayerState::CalculateAndBroadcastXPProgress()
{
	if (!LevelUpInfo) return;

	float CurrentLevelRequirement = LevelUpInfo->GetXPRequirementForLevel(Level);      
	float NextLevelRequirement = LevelUpInfo->GetXPRequirementForLevel(Level + 1); 

	float XPThisLevel = FMath::Max(0.f, XP - CurrentLevelRequirement);
	float XPRequirementForNextLevel = FMath::Max(0.1f, NextLevelRequirement - CurrentLevelRequirement);

	OnXPWindowChangedDelegate.Broadcast(XPThisLevel, XPRequirementForNextLevel);

	float XPPercent = XPThisLevel / XPRequirementForNextLevel;
	XPPercent = FMath::Clamp(XPPercent, 0.f, 1.f);
	OnXPPercentChangedDelegate.Broadcast(XPPercent);
}
void AVS_PlayerState::OnRep_Level(int32 OldLevel)
{
	int32 TempLevel = OldLevel;
	while (TempLevel < Level)
	{
		TempLevel++;
		OnLevelChangedDelegate.Broadcast(TempLevel);
	}

	CalculateAndBroadcastXPProgress();
}

void AVS_PlayerState::OnRep_XP(float OldXP)
{
	CalculateAndBroadcastXPProgress();
}

void AVS_PlayerState::OnRep_RerollCount(int32 OldRerollCount)
{
	// 留给 WidgetController 监听并更新重抽按钮状态
}

void AVS_PlayerState::OnRep_KillCount(int32 OldKillCount)
{
	OnKillCountChangedDelegate.Broadcast(KillCount);
}

void AVS_PlayerState::OnRep_Gold(int32 OldGold)
{
	OnGoldChangedDelegate.Broadcast(Gold);
}
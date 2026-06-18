#include "Game/VS_GameMode.h"
#include "VS_GameInstance.h"
#include "Player/VS_PlayerController.h"
#include "Player/VS_PlayerState.h"
#include "AbilitySystem/VS_AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"

void AVS_GameMode::TravelToGameMap()
{
	UGameplayStatics::OpenLevel(this, FName("VS_GameMap"));
}

void AVS_GameMode::TravelToMainMenuMap()
{
	UGameplayStatics::OpenLevel(this, FName("VS_MainMenuMap"));
}

bool AVS_GameMode::ProcessItemPurchase(FGameplayTag ItemTag, AVS_PlayerController* PC)
{
	if (!ToolPriceData || !PC) return false;

	// 1. 查价格
	int32 Price = ToolPriceData->GetPriceForTag(ItemTag);
	if (Price <= 0) return false; // 找不到价格，或者免费，均阻止交易

	if (UVS_GameInstance* GI = GetGameInstance<UVS_GameInstance>())
	{
		// 2. 查重：是否已经买过？
		if (GI->HasItem(ItemTag))
		{
			UE_LOG(LogTemp, Warning, TEXT("购买失败：已经拥有该物品"));
			return false;
		}

		// 3. 扣费
		bool bSuccess = GI->SpendGold(Price);
		if (bSuccess)
		{
			// 4. 发货存盘
			GI->AddOwnedItem(ItemTag);
			UE_LOG(LogTemp, Log, TEXT("购买成功！花费 %d"), Price);
			return true;
		}
	}

	return false;
}

void AVS_GameMode::ProcessMatchEndAndTravel(AVS_PlayerController* PC)
{
	if (!PC) return;

	AVS_PlayerState* VSPS = PC->GetPlayerState<AVS_PlayerState>();
	UVS_GameInstance* GI = GetGameInstance<UVS_GameInstance>();

	if (VSPS && GI)
	{
		// 1. 创建暂存壳
		FVSMatchResult MatchResult;
		MatchResult.MatchLevel = VSPS->GetPlayerLevel();
		MatchResult.MatchXP = VSPS->GetXP();
		MatchResult.MatchKillCount = VSPS->GetKillCount();
		MatchResult.MatchGold = VSPS->GetGold();

		// 2.取出全部获得的技能！
		if (UVS_AbilitySystemComponent* ASC = Cast<UVS_AbilitySystemComponent>(VSPS->GetAbilitySystemComponent()))
		{
			MatchResult.MatchAbilities = ASC->GetOwnedAbilities();
		}

		// 3. 把这份战报塞进GameInstance
		GI->SetLastMatchResult(MatchResult);

		// 4. 将这局打出的全部金币，注入到账户并落盘
		GI->AddTotalGold(MatchResult.MatchGold);

		UE_LOG(LogTemp, Log, TEXT("结算数据封箱完毕，前往 SettlementMap！"));
	}

	// 5. 数据处理完毕，所有在当前世界的人全部销毁，起飞前往 SettlementMap！
	UGameplayStatics::OpenLevel(this, FName("VS_SettlementMap"));
}
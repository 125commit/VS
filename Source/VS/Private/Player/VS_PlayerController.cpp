#include "Player/VS_PlayerController.h"
#include "Player/VS_PlayerState.h" 
#include "AbilitySystem/VS_AbilitySystemComponent.h"
#include "VSGameplayTags.h" 
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Game/VS_GameMode.h"
#include "GameFramework/Pawn.h"

AVS_PlayerController::AVS_PlayerController()
{
	bReplicates = true;
}

void AVS_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputModeData;
	SetInputMode(InputModeData);
	bShowMouseCursor = false;

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (VSContext)
		{
			Subsystem->AddMappingContext(VSContext, 0);
		}
	}
}

void AVS_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVS_PlayerController::Move);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AVS_PlayerController::PauseButtonPressed);
	}
}

void AVS_PlayerController::Move(const FInputActionValue& InputActionValue)
{
	FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(FVector(1.f, 0.f, 0.f), InputAxisVector.Y);
		ControlledPawn->AddMovementInput(FVector(0.f, 1.f, 0.f), InputAxisVector.X);
	}
}

void AVS_PlayerController::PauseButtonPressed()
{
	FInputModeGameAndUI InputModeData;
	SetInputMode(InputModeData);
	bShowMouseCursor = true;

	Server_PauseGame();

	if (IsLocalController())
	{
		OnShowPauseMenuDelegate.Broadcast();
	}
}

// ==========================================================
// 发牌器算法：依托 GameplayTag 的防作弊过滤
// ==========================================================
TArray<FGameplayTag> AVS_PlayerController::GenerateValidAbilityPool()
{
	TArray<FGameplayTag> ValidPool;

	AVS_PlayerState* VSPS = GetPlayerState<AVS_PlayerState>();
	if (!VSPS || !VSPS->GetAbilitySystemComponent() || !AbilityInfoData)
	{
		return ValidPool;
	}

	UVS_AbilitySystemComponent* ASC = Cast<UVS_AbilitySystemComponent>(VSPS->GetAbilitySystemComponent());

	// 1. 调用 ASC 接口，拿到玩家身上所有的技能及其等级
	TArray<FVSOwnedAbilityInfo> OwnedAbilities = ASC->GetOwnedAbilities();
	
	int32 WeaponCount = 0;
	int32 PassiveCount = 0;
	
	// 2. 解析统计当前已占用的武器槽和被动槽
	for (const FVSOwnedAbilityInfo& OwnedInfo : OwnedAbilities)
	{
		if (OwnedInfo.AbilityTypeTag.MatchesTagExact(FVSGameplayTags::Get().Abilities_Type_Weapon))
		{
			WeaponCount++;
		}
		else if (OwnedInfo.AbilityTypeTag.MatchesTagExact(FVSGameplayTags::Get().Abilities_Type_Passive))
		{
			PassiveCount++;
		}
	}

	const int32 MaxWeaponSlots = 3;
	const int32 MaxPassiveSlots = 3;

	// 3. 遍历技能字典，进行卡池清洗
	for (const FVSAbilityInfo& Info : AbilityInfoData->AbilityInformation)
	{
		// 如果该技能是进化武器本身，绝不能直接刷出
		if (Info.bIsEvolvedWeapon)
		{
			continue;
		}

		// 检查防刷出已进化的废弃武器（被吃掉的祭品）
		if (ASC->EvolvedHistoryWeapons.Contains(Info.AbilityTag))
		{
			continue;
		}

		bool bAlreadyOwned = false;
		int32 CurrentLevel = 0;

		for (const FVSOwnedAbilityInfo& OwnedInfo : OwnedAbilities)
		{
			if (OwnedInfo.AbilityTag.MatchesTagExact(Info.AbilityTag))
			{
				bAlreadyOwned = true;
				CurrentLevel = OwnedInfo.Level;
				break;
			}
		}

		// 如果已经练过，且练到满级了（>= MaxLevel），踢出卡池
		if (bAlreadyOwned && CurrentLevel >= Info.MaxLevel)
		{
			continue;
		}

		// 如果没练过，但是对应的背包格子满了，踢出卡池
		if (!bAlreadyOwned)
		{
			if (Info.AbilityTypeTag.MatchesTagExact(FVSGameplayTags::Get().Abilities_Type_Weapon) && WeaponCount >= MaxWeaponSlots)
			{
				continue;
			}
			if (Info.AbilityTypeTag.MatchesTagExact(FVSGameplayTags::Get().Abilities_Type_Passive) && PassiveCount >= MaxPassiveSlots)
			{
				continue;
			}
		}

		// 校验通过，入池
		ValidPool.Add(Info.AbilityTag);
	}

	return ValidPool;
}

// ==========================================================
// 服务端黑盒：暂停管线
// ==========================================================

void AVS_PlayerController::Server_PauseGame_Implementation()
{
	SetPause(true);
}

void AVS_PlayerController::Server_ResumeGame_Implementation()
{
	SetPause(false);
}

void AVS_PlayerController::Server_QuitToSettlement_Implementation()
{
	SetPause(false);
	
	if (AVS_GameMode* GM = GetWorld()->GetAuthGameMode<AVS_GameMode>())
	{
		GM->ProcessMatchEndAndTravel(this);
	}
}

// ==========================================================
// 服务端黑盒：升级与重抽
// ==========================================================

void AVS_PlayerController::Server_HandleLevelUp_Implementation()
{
	SetPause(true);

	TArray<FGameplayTag> ValidPool = GenerateValidAbilityPool();

	if (ValidPool.IsEmpty())
	{
		if (AVS_PlayerState* VSPS = GetPlayerState<AVS_PlayerState>())
		{
			int32 Remaining = VSPS->GetPendingLevelUps();
			VSPS->AddGold(100 * Remaining);
			for (int32 k = 0; k < Remaining; ++k)
			{
				VSPS->ConsumePendingLevelUp();
			}
		}
		SetPause(false);
		return;
	}

	TArray<FGameplayTag> Options; 
	
	// 1. 标准洗牌算法 
	for (int32 i = ValidPool.Num() - 1; i > 0; --i)
	{
		int32 j = FMath::RandRange(0, i);
		ValidPool.Swap(i, j);
	}

	// 2. 抽取最多 3 个 (如果池子里只剩 2 个，那就抽 2 个)
	int32 NumToSelect = FMath::Min(3, ValidPool.Num());
	for (int32 i = 0; i < NumToSelect; ++i)
	{
		Options.Add(ValidPool[i]);
	}
	
	Client_ShowLevelUpScreen(Options);
}

void AVS_PlayerController::Server_RerollUpgrade_Implementation()
{
	AVS_PlayerState* VSPS = GetPlayerState<AVS_PlayerState>();
	
	if (VSPS && VSPS->GetRerollCount() > 0)
	{
		VSPS->ConsumeRerollCount();

		TArray<FGameplayTag> ValidPool = GenerateValidAbilityPool();

		if (ValidPool.IsEmpty())
		{
			int32 Remaining = VSPS->GetPendingLevelUps();
			VSPS->AddGold(100 * Remaining);
			for (int32 k = 0; k < Remaining; ++k)
			{
				VSPS->ConsumePendingLevelUp();
			}
			SetPause(false);
			return;
		}

		TArray<FGameplayTag> Options; 
		
		// 1. 洗牌算法
		for (int32 i = ValidPool.Num() - 1; i > 0; --i)
		{
			int32 j = FMath::RandRange(0, i);
			ValidPool.Swap(i, j);
		}

		// 2. 抽取最多 3 个
		int32 NumToSelect = FMath::Min(3, ValidPool.Num());
		for (int32 i = 0; i < NumToSelect; ++i)
		{
			Options.Add(ValidPool[i]);
		}

		Client_ShowLevelUpScreen(Options);
	}
}

void AVS_PlayerController::Server_SelectUpgrade_Implementation(FGameplayTag SelectedTag)
{
	AVS_PlayerState* VSPS = GetPlayerState<AVS_PlayerState>();
	if (VSPS && AbilityInfoData)
	{
		UVS_AbilitySystemComponent* ASC = Cast<UVS_AbilitySystemComponent>(VSPS->GetAbilitySystemComponent());
		
		FVSAbilityInfo SelectedInfo = AbilityInfoData->FindAbilityInfoForTag(SelectedTag);
		
		if (ASC && SelectedInfo.AbilityClass)
		{
			// 发放技能或升级
			ASC->ServerUpgradeAbility(SelectedInfo.AbilityClass);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Server_SelectUpgrade: 无法找到技能配置，或 AbilityClass 为空！Tag: %s"), *SelectedTag.ToString());
		}
			
		// 无论技能是否发放成功，都必须扣减升级次数，否则遇到配置错误时会永久破坏升级队列
		VSPS->ConsumePendingLevelUp();
			
		// 防连发：如果还有升级次数，继续开盲盒
		if (VSPS->GetPendingLevelUps() > 0)
			{
				TArray<FGameplayTag> ValidPool = GenerateValidAbilityPool();

				if (ValidPool.IsEmpty())
				{
					int32 Remaining = VSPS->GetPendingLevelUps();
					VSPS->AddGold(100 * Remaining);
					for (int32 k = 0; k < Remaining; ++k)
					{
						VSPS->ConsumePendingLevelUp();
					}
					SetPause(false);
					return;
				}

				TArray<FGameplayTag> Options; 
				
				// 洗牌并抽卡
				for (int32 i = ValidPool.Num() - 1; i > 0; --i)
				{
					int32 j = FMath::RandRange(0, i);
					ValidPool.Swap(i, j);
				}
				int32 NumToSelect = FMath::Min(3, ValidPool.Num());
				for (int32 i = 0; i < NumToSelect; ++i)
				{
					Options.Add(ValidPool[i]);
				}

				Client_ShowLevelUpScreen(Options);
				return; 
			}
	}

	SetPause(false);
}

// ==========================================================
// 服务端黑盒：宝箱管线
// ==========================================================

void AVS_PlayerController::Server_ProcessChestPickup_Implementation(int32 MaxItems)
{
	SetPause(true);

	TArray<FGameplayTag> ValidPool = GenerateValidAbilityPool();

	FGameplayTag AwardedTag;
	if (ValidPool.Num() > 0)
	{
		// 随机抽取 1 个技能
		int32 RandomIndex = FMath::RandRange(0, ValidPool.Num() - 1);
		AwardedTag = ValidPool[RandomIndex];
	}

	// 算法：以 MaxItems 为最大值，随机奖励一半到满额的金币
	int32 SafeMax = FMath::Max(1, MaxItems);
	int32 GoldAmount = FMath::RandRange(FMath::Max(1, SafeMax / 2), SafeMax);

	AVS_PlayerState* VSPS = GetPlayerState<AVS_PlayerState>();
	if (VSPS && AbilityInfoData)
	{
		// 1. 发放金币
		VSPS->AddGold(GoldAmount);
		
		// 2. 自动给玩家发这张牌 / 升该技能一级
		if (AwardedTag.IsValid())
		{
			UVS_AbilitySystemComponent* ASC = Cast<UVS_AbilitySystemComponent>(VSPS->GetAbilitySystemComponent());
			FVSAbilityInfo SelectedInfo = AbilityInfoData->FindAbilityInfoForTag(AwardedTag);
			
			if (ASC && SelectedInfo.AbilityClass)
			{
				ASC->ServerUpgradeAbility(SelectedInfo.AbilityClass);
			}
		}
	}
	
	Client_ShowChestScreen(GoldAmount, AwardedTag);
}

// ==========================================================
// 服务端：局外管线
// ==========================================================

void AVS_PlayerController::Server_AcceptSettlement_Implementation()
{
	if (AVS_GameMode* GM = GetWorld()->GetAuthGameMode<AVS_GameMode>())
	{
		GM->TravelToMainMenuMap();
	}
}

void AVS_PlayerController::Server_BuyItem_Implementation(FGameplayTag ItemTag)
{
	if (AVS_GameMode* GM = GetWorld()->GetAuthGameMode<AVS_GameMode>())
	{
		GM->ProcessItemPurchase(ItemTag, this);
	}
}

void AVS_PlayerController::Server_StartGame_Implementation()
{
	if (AVS_GameMode* GM = GetWorld()->GetAuthGameMode<AVS_GameMode>())
	{
		GM->TravelToGameMap();
	}
}

// ==========================================================
// 客户端喇叭 (Client Pipeline)
// ==========================================================

void AVS_PlayerController::Client_ShowLevelUpScreen_Implementation(const TArray<FGameplayTag>& SkillOptions)
{
	FInputModeGameAndUI InputModeData;
	SetInputMode(InputModeData);
	bShowMouseCursor = true;

	TArray<FVSAbilityInfo> RichSkillOptions;
	if (AbilityInfoData)
	{
		for (const FGameplayTag& Tag : SkillOptions)
		{
			FVSAbilityInfo Info = AbilityInfoData->FindAbilityInfoForTag(Tag);
			RichSkillOptions.Add(Info);
		}
	}

	OnShowLevelUpMenuDelegate.Broadcast(RichSkillOptions);
}

void AVS_PlayerController::Client_ShowChestScreen_Implementation(int32 GoldAmount, FGameplayTag AwardedTag, bool bIsEvolution, FGameplayTag EvolvedTag)
{
	FInputModeGameAndUI InputModeData;
	SetInputMode(InputModeData);
	bShowMouseCursor = true;

	FVSAbilityInfo AwardedSkillInfo;
	if (AbilityInfoData)
	{
		AwardedSkillInfo = AbilityInfoData->FindAbilityInfoForTag(AwardedTag);
	}

	FVSAbilityInfo EvolvedInfo; if (AbilityInfoData && bIsEvolution) { EvolvedInfo = AbilityInfoData->FindAbilityInfoForTag(EvolvedTag); }
	OnShowChestMenuDelegate.Broadcast(GoldAmount, AwardedSkillInfo, bIsEvolution, EvolvedInfo);
}

#include "Player/VS_PlayerController.h"
#include "Player/VS_PlayerState.h" 
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
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
	// 任务：向 GM 发起结算申请
}

// ==========================================================
// 服务端黑盒：升级与重抽 (Server Pipeline)
// ==========================================================

void AVS_PlayerController::Server_HandleLevelUp_Implementation()
{
	SetPause(true);

	TArray<FGameplayTag> Options; 
	if (AbilityInfoData)
	{
		// 任务：执行暗箱抽卡，抽出 3 个不重复的新 Tag
	}

	Client_ShowLevelUpScreen(Options);
}

void AVS_PlayerController::Server_RerollUpgrade_Implementation()
{
	AVS_PlayerState* VSPS = GetPlayerState<AVS_PlayerState>();
	
	if (VSPS && VSPS->GetRerollCount() > 0)
	{
		VSPS->ConsumeRerollCount();

		TArray<FGameplayTag> Options; 
		if (AbilityInfoData)
		{
			// 任务：重新执行暗箱抽卡，抽出 3 个不重复的新 Tag
		}

		Client_ShowLevelUpScreen(Options);
	}
}

void AVS_PlayerController::Server_SelectUpgrade_Implementation(FGameplayTag SelectedTag)
{
	AVS_PlayerState* VSPS = GetPlayerState<AVS_PlayerState>();
	if (VSPS)
	{
		// 任务：给玩家发技能...
		// 并扣减 PS 里的 PendingLevelUps 
	}

	SetPause(false);
}

// ==========================================================
// 服务端黑盒：宝箱管线
// ==========================================================

void AVS_PlayerController::Server_ProcessChestPickup_Implementation()
{
	SetPause(true);

	FGameplayTag AwardedTag;
	if (AbilityInfoData)
	{
		// 任务：查表抽卡，随机奖励一个技能
	}

	int32 GoldAmount = FMath::RandRange(100, 500);

	Client_ShowChestScreen(GoldAmount, AwardedTag);
}

// ==========================================================
// 服务端黑盒：局外管线
// ==========================================================

void AVS_PlayerController::Server_AcceptSettlement_Implementation()
{
	// 任务：向 GameMode 申请切换关卡
}

void AVS_PlayerController::Server_BuyItem_Implementation(FGameplayTag ItemTag)
{
	// 任务：向 GameMode 申请调用购买计算及存档逻辑
}

void AVS_PlayerController::Server_StartGame_Implementation()
{
	// 任务：向 GameMode 申请调用开始游戏的逻辑
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

void AVS_PlayerController::Client_ShowChestScreen_Implementation(int32 GoldAmount, FGameplayTag AwardedTag)
{
	FInputModeGameAndUI InputModeData;
	SetInputMode(InputModeData);
	bShowMouseCursor = true;

	FVSAbilityInfo AwardedSkillInfo;
	if (AbilityInfoData)
	{
		AwardedSkillInfo = AbilityInfoData->FindAbilityInfoForTag(AwardedTag);
	}

	OnShowChestMenuDelegate.Broadcast(GoldAmount, AwardedSkillInfo);
}
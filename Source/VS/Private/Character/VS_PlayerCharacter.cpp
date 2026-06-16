// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/VS_PlayerCharacter.h"
#include "AbilitySystem/VS_AttributeSet.h"
#include "Player/VS_PlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/VS_AbilitySystemComponent.h"

AVS_PlayerCharacter::AVS_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVS_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server端附身时初始化 GAS
	InitAbilityActorInfo();
}

void AVS_PlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Client端 PlayerState 同步到位时初始化 GAS
	InitAbilityActorInfo();
}

void AVS_PlayerCharacter::InitAbilityActorInfo()
{
	AVS_PlayerState* VSPlayerState = GetPlayerState<AVS_PlayerState>();
	if (VSPlayerState)
	{
		// 绑定 GAS 的 Owner 为 PlayerState，Avatar 为当前的 PlayerCharacter (this)
		VSPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(VSPlayerState, this);
		
		AbilitySystemComponent = VSPlayerState->GetAbilitySystemComponent(); // 设置角色中的ASC指针
		AttributeSet = VSPlayerState->GetAttributeSet(); 
		Cast<UVS_AbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	}
}

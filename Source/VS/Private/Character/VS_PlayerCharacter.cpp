// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/VS_PlayerCharacter.h"
#include "AbilitySystem/VS_AttributeSet.h"
#include "Player/VS_PlayerState.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/VS_AbilitySystemComponent.h"
#include "Data/DA_CharacterClassInfo.h"

AVS_PlayerCharacter::AVS_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AVS_PlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server端附身时初始化 GAS
	InitAbilityActorInfo();

	// 根据设计师在蓝图选择的 CharacterClass，去大字典里抓取技能和属性
	if (HasAuthority() && CharacterClassInfo)
	{
		AVS_PlayerState* VSPlayerState = GetPlayerState<AVS_PlayerState>();
		if (VSPlayerState)
		{
			if (UVS_AbilitySystemComponent* VSASC = Cast<UVS_AbilitySystemComponent>(VSPlayerState->GetAbilitySystemComponent()))
			{
				FCharacterClassDefaultInfo ClassInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

				// 1. 更换专属骨骼网格体
				if (ClassInfo.CharacterMesh)
				{
					GetMesh()->SetSkeletalMesh(ClassInfo.CharacterMesh);
				}

				// 2. 赋予初始武器/被动技能
				VSASC->AddCharacterAbilities(ClassInfo.StartupAbilities);

				// 3. 赋予初始属性 (比如给特定英雄加最大血量)
				if (ClassInfo.PrimaryAttributes)
				{
					FGameplayEffectContextHandle ContextHandle = VSASC->MakeEffectContext();
					ContextHandle.AddSourceObject(this);
					FGameplayEffectSpecHandle SpecHandle = VSASC->MakeOutgoingSpec(ClassInfo.PrimaryAttributes, 1.0f, ContextHandle);
					if (SpecHandle.IsValid())
					{
						VSASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
		}
	}
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
	
	if (HasAuthority())
	{
		InitializeVitalAttributes();
	}
}

void AVS_PlayerCharacter::InitializeVitalAttributes() const
{
	
	if (!DefaultVitalAttributes || !GetAbilitySystemComponent()) return;
	
	// 创建效果上下文句柄
	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	// 使用默认游戏效果类生成效果规格句柄，等级为 1.0
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(DefaultVitalAttributes, 1.f, ContextHandle);
	// 将效果规格应用到目标（自身）；必须从句柄中取出实际规格并解引用为常引用
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

UAbilitySystemComponent* AVS_PlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

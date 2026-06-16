// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/VSCharacterBase.h"
#include "VS_PlayerCharacter.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

class UGameplayAbility;
/**
 * 玩家主角类
 */
UCLASS()
class VS_API AVS_PlayerCharacter : public AVSCharacterBase
{
	GENERATED_BODY()

public:
	AVS_PlayerCharacter();

	// 服务端初始化：被 Controller 附身时调用
	virtual void PossessedBy(AController* NewController) override;

	// 客户端初始化：PlayerState 属性同步到客户端时调用
	virtual void OnRep_PlayerState() override;

private:
	// 统一的 GAS 初始化挂载入口 (将 ASC 的 Owner 设置为 PS，Avatar 设置为自身)
	void InitAbilityActorInfo();
	
protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent; // ASC 指针
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet; // AS 指针
	
	// 角色天生自带的技能 (可以在蓝图里配置)
	UPROPERTY(EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
};

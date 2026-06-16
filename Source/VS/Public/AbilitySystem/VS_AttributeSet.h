// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "VS_AttributeSet.generated.h"


// 宏封装：一键生成 Get、Set、Init 四个函数
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


/**
 * 
 */
UCLASS()
class VS_API UVS_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UVS_AttributeSet();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	// [防溢出] 修改发生前：用于硬性基础范围限制（如血量不能 < 0）
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
	// [防溢出：元属性清零] GE 结算后：用于最终生命值钳制，并消耗掉传入的元属性
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
	// 元属性 (Meta Attributes) 不参与网络同步！仅作临时数据信道
	// =============================================================
	
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UVS_AttributeSet, IncomingDamage);

	/* -------------------- 核心属性声明 -------------------- */
	// 1. 当前生命值
	UPROPERTY(BlueprintReadOnly,ReplicatedUsing=OnRep_Health,Category="Vital Attrubutes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UVS_AttributeSet, Health);
	
	// 2. 最大生命值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UVS_AttributeSet, MaxHealth);
	
	// 3. 基础伤害倍率
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Vital Attributes")
	FGameplayAttributeData BaseDamage;
	ATTRIBUTE_ACCESSORS(UVS_AttributeSet, BaseDamage);
	
	// 4. 拾取范围(专供 DropManager 纯数学轮询比对)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagnetRadius, Category = "Vital Attributes")
	FGameplayAttributeData MagnetRadius;
	ATTRIBUTE_ACCESSORS(UVS_AttributeSet, MagnetRadius);
	
	// 5. 武器冷却
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WeaponCooldown, Category = "Vital Attributes")
	FGameplayAttributeData WeaponCooldown;
	ATTRIBUTE_ACCESSORS(UVS_AttributeSet, WeaponCooldown);
	
	// 6.武器持续时间
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WeaponDuration , Category = "Vital Attributes")
	FGameplayAttributeData WeaponDuration ;
	ATTRIBUTE_ACCESSORS(UVS_AttributeSet, WeaponDuration );
	
	//7.武器范围倍率
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WeaponArea, Category = "Vital Attributes")
	FGameplayAttributeData WeaponArea;
	ATTRIBUTE_ACCESSORS(UVS_AttributeSet, WeaponArea);

	//8.贪婪/经验加成(经验获取加成比)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_GreedMultiplier, Category = "Vital Attributes")
	FGameplayAttributeData GreedMultiplier;
	ATTRIBUTE_ACCESSORS(UVS_AttributeSet, GreedMultiplier);
	
	/* -------------------- 网络回调声明 -------------------- */
	
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldBaseDamage) const;
	UFUNCTION()
	void OnRep_MagnetRadius(const FGameplayAttributeData& OldMagnetRadius) const;
	UFUNCTION()
	void OnRep_WeaponCooldown(const FGameplayAttributeData& OldWeaponCooldown) const;
	UFUNCTION()
	void OnRep_WeaponDuration(const FGameplayAttributeData& OldWeaponDuration) const;
	UFUNCTION()
	void OnRep_WeaponArea(const FGameplayAttributeData& OldWeaponArea) const;
	UFUNCTION()
	void OnRep_GreedMultiplier(const FGameplayAttributeData& OldGreedMultiplier) const;
};

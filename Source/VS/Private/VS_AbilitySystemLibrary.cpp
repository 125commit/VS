#include "VS_AbilitySystemLibrary.h"
#include "AbilitySystem/VS_AbilitySystemComponent.h"
#include "Data/DA_CharacterClassInfo.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/VS_AttributeSet.h"

void UVS_AbilitySystemLibrary::InitializeDefaultAttributesAndAbilities(UObject* WorldContextObject, UVS_AbilitySystemComponent* VSASC, UDA_CharacterClassInfo* CharacterClassInfo, ECharacterClass CharacterClass)
{
	if (!VSASC || !CharacterClassInfo) return;

	// 1. 查表，拿到对应英雄的数据包
	FCharacterClassDefaultInfo DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	
	// 2. 灌注初始武器 (调用上一节的 AddCharacterAbilities，自动触发 TryActivateAbility 死循环)
	VSASC->AddCharacterAbilities(DefaultInfo.StartupAbilities);

	// 3. 灌注初始属性加成 (应用 GE)
	if (DefaultInfo.PrimaryAttributes)
	{
		// 构建 Effect Context，把 AvatarActor 放进去防止断言崩溃
		FGameplayEffectContextHandle ContextHandle = VSASC->MakeEffectContext();
		ContextHandle.AddSourceObject(VSASC->GetAvatarActor());

		// 生成 Spec 并应用给自身
		FGameplayEffectSpecHandle SpecHandle = VSASC->MakeOutgoingSpec(DefaultInfo.PrimaryAttributes, 1.f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			VSASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

float UVS_AbilitySystemLibrary::HealActor(AActor* TargetActor, float HealAmount)
{
	if (!TargetActor || HealAmount <= 0.f) return 0.f;
	
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!ASC) return 0.f;
	
	const float OldHealth = ASC->GetNumericAttribute(UVS_AttributeSet::GetHealthAttribute());
	const float MaxHealth = ASC->GetNumericAttribute(UVS_AttributeSet::GetMaxHealthAttribute());
	const float NewHealth = FMath::Clamp(OldHealth + HealAmount, 0.f, MaxHealth);
	
	ASC->SetNumericAttributeBase(UVS_AttributeSet::GetHealthAttribute(), NewHealth);
	
	return NewHealth - OldHealth;
}



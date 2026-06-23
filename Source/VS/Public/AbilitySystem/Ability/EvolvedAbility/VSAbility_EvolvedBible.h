// LAvid

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Ability/VSAbility_Bible.h"
#include "VSAbility_EvolvedBible.generated.h"

/**
 * 
 */
UCLASS()
class VS_API UVSAbility_EvolvedBible : public UVSAbility_Bible
{
	GENERATED_BODY()
	
	
public:
	UVSAbility_EvolvedBible();
	
protected:
	// 取消"在场→空窗→再出现"的闪烁节奏，让书常驻旋转
	virtual float GetNextFireDelay(const FVSAbilityRuntimeStats& Stats) const override;
};


#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/DA_CharacterClassInfo.h" 
#include "VS_AbilitySystemLibrary.generated.h"

class UVS_AbilitySystemComponent;

UCLASS()
class VS_API UVS_AbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	// 核心桥梁：读取职业表，提取武器和属性，塞给 ASC
	UFUNCTION(BlueprintCallable, Category = "VSAbilitySystemLibrary|CharacterClassInfo")
	static void InitializeDefaultAttributesAndAbilities(UObject* WorldContextObject, UVS_AbilitySystemComponent* VSASC, UDA_CharacterClassInfo* CharacterClassInfo, ECharacterClass CharacterClass);
};

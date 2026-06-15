#include "Data/DA_AbilityInfo.h"
#include "VS/VS.h" 

FVSAbilityInfo UDA_AbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const FVSAbilityInfo& Info : AbilityInformation)
	{
		// 精确匹配查表
		if (Info.AbilityTag.MatchesTagExact(AbilityTag))
		{
			return Info;
		}
	}

	// 如果没找到且允许报错，则打印红字警告
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("无法在 AbilityInfo [%s] 中找到 Tag 为 [%s] 的技能信息"), *GetNameSafe(this), *AbilityTag.ToString());
	}

	// 没找到则返回一个空的结构体
	return FVSAbilityInfo();
}
#include "Data/DA_LevelUpInfo.h"

int32 UDA_LevelUpInfo::FindLevelForXP(float XP) const
{
	int32 Level = 1;
	bool bSearching = true;

	// 遍历数组，找到第一个要求经验大于当前经验的等级
	while (bSearching)
	{
		// 如果查超出了策划配置的数组最大长度，就直接返回最高级
		if (LevelUpInformation.Num() - 1 <= Level) return Level;

		if (XP >= LevelUpInformation[Level].XPRequirement)
		{
			Level++;
		}
		else
		{
			bSearching = false;
		}
	}

	return Level;
}

float UDA_LevelUpInfo::GetXPRequirementForLevel(int32 Level) const
{
	// 安全校验：防止越界崩溃
	if (LevelUpInformation.IsValidIndex(Level))
	{
		return LevelUpInformation[Level].XPRequirement;
	}
	
	// 如果越界（比如策划忘了填），默认返回一个极大值
	return 999999.f; 
}
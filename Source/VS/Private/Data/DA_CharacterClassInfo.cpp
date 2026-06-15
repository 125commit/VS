#include "Data/DA_CharacterClassInfo.h"


FCharacterClassDefaultInfo UDA_CharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass)
{
	if (CharacterClassInformation.Contains(CharacterClass))
	{
		return CharacterClassInformation[CharacterClass];
	}
	
	return FCharacterClassDefaultInfo();
}
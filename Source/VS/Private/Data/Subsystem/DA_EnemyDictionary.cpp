#include "Data/Subsystem/DA_EnemyDictionary.h"
#include "Character/VSEnemy.h"

TSubclassOf<AVSEnemy> UDA_EnemyDictionary::GetEnemyClass(EVSEnemyType Type) const
{
	if (const TSubclassOf<AVSEnemy>* FoundClass = EnemyClassMap.Find(Type))
	{
		return *FoundClass;
	}
	return nullptr;
}

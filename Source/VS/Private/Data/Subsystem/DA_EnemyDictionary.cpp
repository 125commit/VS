#include "Data/Subsystem/DA_EnemyDictionary.h"
#include "Character/VSEnemy.h"


bool UDA_EnemyDictionary::GetEnemyDefinition(EVSEnemyType Type, FVSEnemyDefinition& OutDef) const
{
	const FVSEnemyInfo* Info = Enemies.Find(Type);
	if (!Info) return false;
	
	const FVSEnemyStatRow* Row = Info->StatRow.GetRow<FVSEnemyStatRow>(TEXT("GetEnemyDefinition"));
	if (!Row) return false;
	
	OutDef.EnemyClass            = Info->EnemyClass;
	OutDef.DamageEffectClass     = Info->DamageEffectClass;
	OutDef.MaxHealth             = Row->MaxHealth;
	OutDef.MoveSpeed             = Row->MoveSpeed;
	OutDef.BodyScale             = Row->BodyScale;
	OutDef.AttackDamage          = Row->AttackDamage;
	OutDef.AttackInterval        = Row->AttackInterval;
	OutDef.KnockbackResistance   = Row->KnockbackResistance;
	OutDef.EliteHealthMultiplier = Row->EliteHealthMultiplier;
	OutDef.EliteScaleMultiplier  = Row->EliteScaleMultiplier;
	OutDef.DropTable             = Row->DropTable;
	
	return true;
	
}

TSubclassOf<AVSEnemy> UDA_EnemyDictionary::GetEnemyClass(EVSEnemyType Type) const
{
	if (const FVSEnemyInfo* Info = Enemies.Find(Type))
	{
		return Info->EnemyClass;
	}
	return nullptr;
}

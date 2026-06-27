#include "Data/Subsystem/DA_EnemyDictionary.h"
#include "Character/VSEnemy.h"
#include "Character/VSEnemyType.h"
#include "Engine/DataTable.h"
#include "GameplayEffect.h"


bool UDA_EnemyDictionary::GetEnemyDefinition(EVSEnemyType Type, FVSEnemyDefinition& OutDef) const
{
	if (Type == EVSEnemyType::None)
	{
		return false;
	}

	// NOTE: 字典未配置的类型回退到 Ghost，便于只维护一张敌人 BP 也能跑全表
	const FVSEnemyInfo* Info = Enemies.Find(Type);
	if (!Info)
	{
		Info = Enemies.Find(EVSEnemyType::Ghost);
	}
	if (!Info)
	{
		return false;
	}

	// CHANGED: 统一用枚举名查 DT_EnemyInfo，StatRow 仅提供 DataTable 引用
	const UDataTable* StatsTable = Info->StatRow.DataTable.Get();
	if (!StatsTable)
	{
		StatsTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT/Enemy/DT_EnemyInfo"));
	}

	const UEnum* EnemyTypeEnum = StaticEnum<EVSEnemyType>();
	const FName RowName = EnemyTypeEnum
		? FName(*EnemyTypeEnum->GetNameStringByValue(static_cast<int64>(Type)))
		: NAME_None;

	const FVSEnemyStatRow* Row = StatsTable
		? StatsTable->FindRow<FVSEnemyStatRow>(RowName, TEXT("GetEnemyDefinition"))
		: nullptr;
	if (!Row)
	{
		return false;
	}

	OutDef.EnemyClass = Info->EnemyClass;
	if (!OutDef.EnemyClass)
	{
		if (const FVSEnemyInfo* GhostInfo = Enemies.Find(EVSEnemyType::Ghost))
		{
			OutDef.EnemyClass = GhostInfo->EnemyClass;
		}
	}

	OutDef.DamageEffectClass = Info->DamageEffectClass;
	if (!OutDef.DamageEffectClass)
	{
		if (const FVSEnemyInfo* GhostInfo = Enemies.Find(EVSEnemyType::Ghost))
		{
			OutDef.DamageEffectClass = GhostInfo->DamageEffectClass;
		}
	}
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
		if (Info->EnemyClass)
		{
			return Info->EnemyClass;
		}
	}

	if (const FVSEnemyInfo* GhostInfo = Enemies.Find(EVSEnemyType::Ghost))
	{
		return GhostInfo->EnemyClass;
	}

	return nullptr;
}

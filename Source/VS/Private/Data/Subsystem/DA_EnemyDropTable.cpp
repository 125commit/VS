// LAvid

#include "Data/Subsystem/DA_EnemyDropTable.h"

bool UDA_EnemyDropTable::IsDrop(FDropEntry& OutEntry) const
{
	if (DropEntries.Num() == 0) return false;

	for (const FDropEntry& Entry : DropEntries)
	{
		if (Entry.DropType == EVSDropType::None) continue;

		if (Entry.bGuaranteed || FMath::FRand() < Entry.DropChance)
		{
			OutEntry = Entry;
			return true;
		}
	}

	return false;
}

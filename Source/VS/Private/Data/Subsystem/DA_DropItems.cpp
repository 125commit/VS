// LAvid


#include "Data/Subsystem/DA_DropItems.h"


FDropTypeDefinition UDA_DropItems::FindDefinition(EVSDropType Type) const
{
	if (Type == EVSDropType::None) return FDropTypeDefinition();
	for (TPair<EVSDropType, FDropTypeDefinition> Pair: DropTypeToTypeDef)
	{
		if (Pair.Key == Type)
		{
			return Pair.Value;
		}
	}
	return FDropTypeDefinition();
}

TSubclassOf<AVSDropItem> UDA_DropItems::FindDropClass(EVSDropType Type) const
{
	if (Type == EVSDropType::None) return nullptr;
	
	if (DropTypeToTypeDef.Contains(Type))
	{
		FDropTypeDefinition DropTypeDefinition= DropTypeToTypeDef[Type];
		return DropTypeDefinition.DropActorClass;
	}
	return nullptr;
}

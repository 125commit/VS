// LAvid

#include "Data/Subsystem/VSDropData.h"
#include "Actor/VS_DropItemActor.h"

FVSDropTypeDefinition UVS_DropItemDataAsset::FindDefinition(EVSDropType Type) const
{
	if (Type == EVSDropType::None) return FVSDropTypeDefinition();
	for (TPair<EVSDropType, FVSDropTypeDefinition> Pair: DropTypeToTypeDef)
	{
		if (Pair.Key == Type)
		{
			return Pair.Value;
		}
	}
	return FVSDropTypeDefinition();
}

TSubclassOf<AVS_DropItemActor> UVS_DropItemDataAsset::FindDropClass(EVSDropType Type) const
{
	if (Type == EVSDropType::None) return nullptr;
	
	if (DropTypeToTypeDef.Contains(Type))
	{
		FVSDropTypeDefinition DropTypeDefinition = DropTypeToTypeDef[Type];
		return DropTypeDefinition.DropActorClass;
	}
	return nullptr;
}

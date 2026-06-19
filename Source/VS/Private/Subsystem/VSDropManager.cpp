// LAvid

#include "Subsystem/VSDropManager.h"
#include "Pool/VSObjectPool.h"
#include "Actor/VSDropItem.h"
#include "Player/VS_PlayerState.h"
#include "AbilitySystem/VS_AttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Data/Subsystem/DA_DropItems.h"
#include "Data/Subsystem/DA_EnemyDropTable.h"


void UVSDropManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	DropPool = NewObject<UVSObjectPool>(this);

	//初始化设置DropSetting
	DropSettings = Cast<UDA_DropItems>(StaticLoadObject(
		UDA_DropItems::StaticClass(),
		nullptr,
		TEXT("/Game/Data/DA_DropItems.DA_DropItems")
	));
	
	if (!DropSettings)
	{
		UE_LOG(LogTemp, Error, TEXT("VSDropManager: Failed to load DA_DropItems at /Game/Data/DA_DropItems"));
	}
	
	if (DropSettings)
	{
		MagnetRadius = DropSettings->DefaultMagnetRadius;
		MagnetMoveSpeed = DropSettings->MagnetMoveSpeed;
		PickupRadius = DropSettings->PickupRadius;
	}

	MagnetRadiusSq = MagnetRadius * MagnetRadius;

	PreWarmPools();
}

void UVSDropManager::Deinitialize()
{
	TArray<TObjectPtr<AVSDropItem>> DropsToReturn = MoveTemp(ActiveDrops);
	for (AVSDropItem* Drop : DropsToReturn)
	{
		if (!DropPool || !Drop) continue;
		Drop->DeActivateDrop();
		DropPool->ReturnActorToPool(Drop);
	}
	ActiveDrops.Empty();

	if (DropPool)
	{
		DropPool->ClearPool();
	}

	Super::Deinitialize();
}

void UVSDropManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (ActiveDrops.Num() == 0) return;

	TryPickUp(DeltaTime);
}

TStatId UVSDropManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UVSDropManager, STATGROUP_Tickables);
}

void UVSDropManager::SpawnDrop(const FVector& Location, const UDA_EnemyDropTable* DropTable)
{
	UWorld* World = GetWorld();
	if (!World || !DropPool || !DropSettings || !DropTable) return;

	FDropEntry SelectedEntry;
	if (!DropTable->IsDrop(SelectedEntry)) return;

	const FDropTypeDefinition DropTypeDefinition = DropSettings->FindDefinition(SelectedEntry.DropType);
	const TSubclassOf<AVSDropItem> DropClass = DropTypeDefinition.DropActorClass;
	if (DropClass == nullptr) return;

	//Spawn
	AActor* Actor = DropPool->GetActorFromPool(World, DropClass, Location, FRotator::ZeroRotator);
	AVSDropItem* DropItem = Cast<AVSDropItem>(Actor);
	check(DropItem);

	DropItem->ActivateDrop(SelectedEntry.DropType, SelectedEntry.DropValue);
	ActiveDrops.Add(DropItem);
}

void UVSDropManager::TryPickUp(float DeltaTime)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn) return;

	AVS_PlayerState* PlayerState = PlayerPawn->GetPlayerState<AVS_PlayerState>();
	if (!PlayerState) return;

	
	const FVector PlayerLoc = PlayerPawn->GetActorLocation();

	const float CurrentMagnetRadiusSq = GetPlayerMagnetRadiusSq(PlayerPawn);
	const float PickupSq = GetPlayerPickupRadiusSq();

	for (int32 i = ActiveDrops.Num() - 1; i >= 0; --i)
	{
		AVSDropItem* Drop = ActiveDrops[i];
		if (!IsValid(Drop) || !Drop->IsActive())
		{
			ActiveDrops.RemoveAtSwap(i);
			continue;
		}
		const FVector DropLoc = Drop->GetActorLocation();
		const float DistSq = FVector::DistSquared(PlayerLoc, DropLoc);

		if (DistSq <= PickupSq)
		{
			HandleCollectingDrop(Drop, PlayerState);
			continue;
		}

		if (DistSq <= CurrentMagnetRadiusSq)
		{
			FVector Dir = PlayerLoc - DropLoc;
			Dir.Z = 0.f;
			if (!Dir.IsNearlyZero())
			{
				Dir.Normalize();
				Drop->SetActorLocation(DropLoc + Dir * MagnetMoveSpeed * DeltaTime, false);
			}
		}
	}
}

void UVSDropManager::HandleCollectingDrop(AVSDropItem* DropItem, AVS_PlayerState* PlayerState)
{
	if (!DropItem || !DropPool || !PlayerState) return;
	switch (DropItem->GetDropType())
	{
	case EVSDropType::XPSmall:
	case EVSDropType::XPLarge:
		PlayerState->AddXP(DropItem->GetDropValue());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Get XP: [%f]"), DropItem->GetDropValue()));
		break;

	case EVSDropType::Gold:
		break;

	case EVSDropType::Turkey:
		break;

	case EVSDropType::Box:
		break;

	default:
		break;
	}

	DropItem->DeActivateDrop();
	ActiveDrops.RemoveSingleSwap(DropItem);
	DropPool->ReturnActorToPool(DropItem);
}

float UVSDropManager::GetPlayerMagnetRadiusSq(const APawn* PlayerPawn)
{
	if (!PlayerPawn) return MagnetRadiusSq;

	if (AVS_PlayerState* PS = Cast<AVS_PlayerState>(PlayerPawn->GetPlayerState()))
	{
		if (UVS_AttributeSet* AS = PS->GetAttributeSet())
		{
			const float CurrentMagnetRadius = AS->GetMagnetRadius();
			return CurrentMagnetRadius * CurrentMagnetRadius;
		}
	}
	return MagnetRadiusSq;
}

float UVSDropManager::GetPlayerPickupRadiusSq()
{
	return PickupRadius * PickupRadius;
}

void UVSDropManager::PreWarmPools()
{
	if (!DropPool || !DropSettings) return;

	UWorld* World = GetWorld();
	if (!World) return;

	for (const TPair<EVSDropType, FDropTypeDefinition>& Pair : DropSettings->DropTypeToTypeDef)
	{
		const FDropTypeDefinition& Def = Pair.Value;
		if (!Def.DropActorClass || Def.PreSpawnCount <= 0) continue;

		for (int32 i = 0; i < Def.PreSpawnCount; ++i)
		{
			AActor* Actor = DropPool->GetActorFromPool(World, Def.DropActorClass, FVector::ZeroVector, FRotator::ZeroRotator);

			if (AVSDropItem* DropItem = Cast<AVSDropItem>(Actor))
			{
				DropItem->DeActivateDrop();
				DropPool->ReturnActorToPool(DropItem);
			}
		}
	}
}
